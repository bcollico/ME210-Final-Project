#include <Arduino.h>
#include <MPU9250.h>
#include <LineFollowing.hpp>
#include <MotorControl.hpp>
#include <BeaconSensing.hpp>
#include <CrowdPleasing.hpp>
#include <PressDispensing.hpp>
#include <Parameters.hpp>

#define USE_TIMER_2 true
// #define USE_TIMER_1 true

#include <TimerInterrupt.h>
#include <ISR_Timer.h>

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire,0x68);

// counter for respToKey
int counter = 0;

// motor timer
long int motor_timer_start;
long int motor_timer_duration;
// ideally we want to rotate 220 from beacon to where we go forward for line
int lastRed = -1;
bool inStudio = 1;
bool hitLineOnWayThere = 0;
bool hitBlackLine = 0;
bool hitRedLineOnWayBack = 0;

// game timer
long int game_start_time = millis();

// buffer for moving from first press bucket
long int black_tape_start;
long int black_tape_timer;
bool change_to_press = false;

// store beacon frequency
Freqs_t our_freq;


/* ---------- STATES ----------*/
typedef enum {
  BOT_IDLE, FINDING_BEACON, SOFT_TURNING_TO, TURNING,
  PRESS_DISP, GAMEOVER, DRIVE_BWD, SOFT_TURNING_FROM, 
  LINE_FOLLOWING, INTO_STUDIO, RELOADING, STUDIO_BLK,
  DRIVE_FWD_TO, DRIVE_FWD_FROM, FINDING_BEACON_RESTART
} States_t;

typedef enum {LINE_FOLLOW_TO, LINE_FOLLOW_FROM}
LineFollowStates_t;


// FwdStates_t FWD_STATE = STUDIO_BLK;
States_t STATE = BOT_IDLE;
LineFollowStates_t LF_STATE = LINE_FOLLOW_TO;

/*---------------Module Function Prototypes-----------------*/
void checkGlobalEvents(void);
unsigned char TestForKey(void);
void RespToKey(void);

// instantiate line following class
LineFollowing Lines = LineFollowing();

// instantiate motor control class
MotorControl Motors = MotorControl(L_EPIN, R_EPIN, L_DPIN, R_DPIN);

// instantiate beacon sensing class with digital pin 2 as input
BeaconSensing Beacon = BeaconSensing(IR_PIN_IN, (float)45.0);

// instantiate crowd pleaser class
CrowdPleasing CrowdPleaser = CrowdPleasing(CP_PIN);

// instantiate press dispenser class
PressDispensing PressDispenser = PressDispensing(PD_PIN);

// buffering for printing to console.
long int current_millis = millis();
long int timer_start = millis();
int buffer = 200;

// timer interrupt functions for IR beacon sensing
void incrementCounter(){
  Beacon.incrementCounter();
}

void interruptHandler(){
  Beacon.Update();
}

float const PIPI = 2*PI;

// setup
void setup() {
  Serial.begin(115200);
  while(!Serial);

  game_start_time = millis();

  attachInterrupt(digitalPinToInterrupt(Beacon.pin), incrementCounter, FALLING);

  // the beacon frequency estimate will update automatically at the 
  // specified frequency (estimate_freq).
  ITimer2.init();
  ITimer2.attachInterrupt(Beacon.estimate_freq, interruptHandler);

  CrowdPleaser.start(CP_DURATION);

  pinMode(FREQ_SWITCH_PIN, INPUT);

  if (digitalRead(FREQ_SWITCH_PIN)) {
    our_freq = LOW_FREQ;
  } else {
    our_freq = HIGH_FREQ; // TODO: switch to LOW/HIGH_FREQ
  }

  // Motors.slowLeft();

  // Lines.calibrate_sensors();

}

void loop() {

  // Serial.println(STATE);
  // Serial.println(our_freq);

  current_millis = millis();
  Lines.Update();  // call this as frequently as you want to update the averages

  switch (STATE) {
    case BOT_IDLE:
      STATE = FINDING_BEACON;
      // Motors.slowLeft();
      Motors.moveBot(CW,CCW,30,30);
      break;
    case FINDING_BEACON:
      if (Beacon.checkForFrequency(our_freq, INSTANT)){
        Motors.idle();
        motor_timer_start = millis();
        motor_timer_duration = 0; // 225-deg turn        
        STATE = TURNING;
        Motors.idle();
        break;
      }
      // Serial.print(our_freq);
      // Serial.print(", ");
      // Serial.println(Beacon.freq_vals[Beacon.newest_idx]);
      break;
    case STUDIO_BLK:
      if (Lines.checkAnySensor(BLACK)) {
        STATE = DRIVE_FWD_TO;
        motor_timer_start = millis();
        inStudio = 0;
      }
      break;
    case TURNING:
      if ((current_millis - motor_timer_start) >= motor_timer_duration){
        Motors.idle();
        if (LF_STATE == LINE_FOLLOW_TO && inStudio == 1){
          STATE = STUDIO_BLK;
          Motors.moveBot(CCW,CCW,60,60);
          // Motors.forward();
        } else if (LF_STATE == LINE_FOLLOW_TO && inStudio == 0 && hitLineOnWayThere == 0) {
          STATE = LINE_FOLLOWING;
          Motors.forward();
          hitLineOnWayThere = 1;
        }
        else if (LF_STATE == LINE_FOLLOW_TO && inStudio == 0){
          STATE = DRIVE_FWD_TO;
          // Motors.forward();
          Motors.moveBot(CCW,CCW,60,60);
        } else if (LF_STATE == LINE_FOLLOW_FROM && inStudio == 1){
          Motors.idle();
          STATE = RELOADING;
          LF_STATE = LINE_FOLLOW_TO;
        } else if (LF_STATE == LINE_FOLLOW_FROM && inStudio == 0 && hitRedLineOnWayBack == 1) {
          STATE = LINE_FOLLOWING;
          Motors.forward();
          hitRedLineOnWayBack = 0;
        } else {
          // Motors.forward();
          Motors.moveBot(CCW,CCW,60,60);
          motor_timer_start = millis();
          STATE = DRIVE_FWD_FROM;
        }
      }
      break;
    case DRIVE_FWD_TO:
      if ((current_millis-motor_timer_start >= 1500) && Lines.checkAnySensor(RED)){ // GAIN
        // stop, make a left turn, and then go straight
        Motors.idle();
        STATE = TURNING;
        motor_timer_start = millis();
        motor_timer_duration = 342; // 90-deg turn  // GAIN  
        // Motors.slowLeft();
        Motors.moveBot(CW,CCW,35,35);
      }
      if ((current_millis-motor_timer_start >= 2000)){ // TODO: adjust as speed increases // GAIN
        // Motors.moveBot(CCW, CCW, FAST - 10, FAST); // TODO: adjust for speed
        // Motors.forward();
        Motors.moveBot(CCW,CCW,60,60);
        STATE = SOFT_TURNING_TO;
      }
      break;
    case SOFT_TURNING_TO:
      if (Lines.checkSensor(LEFT, RED)) { // TODO: change if line following not aggressive enough
        // if (Lines.checkSensor(LEFT, RED)) {lastRed = LEFT;}
        // else if (Lines.checkSensor(LEFT_MID, RED)) {lastRed = LEFT_MID;}
        // else if (Lines.checkSensor(RIGHT_MID, RED)) {lastRed = RIGHT_MID;}
        // else {lastRed = RIGHT;}
        lastRed = 10; // maybe temporary? To null out any lastRed behavior after turn
        motor_timer_start = millis();
        motor_timer_duration = 342;  // GAIN     
        STATE = TURNING;
        // Motors.slowLeft();
        Motors.moveBot(CW,CCW,35,35);
        }
      break;
    case LINE_FOLLOWING:
      if (Lines.checkAnySensor(BLACK) && LF_STATE == LINE_FOLLOW_TO) {
        Motors.idle();
        // if (lastRed == LEFT) {
        //   Motors.moveBot(CW,CCW,35,35);
        //   delay(300);
        //   Motors.idle();
        // } else if (lastRed == RIGHT) {
        //   Motors.moveBot(CCW,CW,35,35);
        //   delay(300);
        //   Motors.idle();
        // }
        STATE = PRESS_DISP;
        PressDispenser.start(PD_DURATION);
        LF_STATE = LINE_FOLLOW_FROM;
        delay(10);
      } else if (Lines.checkAnySensor(BLACK) && LF_STATE == LINE_FOLLOW_FROM) {
          //Serial.println("REACHED STUDIO LINE.");
          // Motors.idle();
          // delay(5000);
          Motors.moveBot(CCW, CCW, FAST - 8, FAST); // TODO: adjust for speed // GAIN
          STATE = INTO_STUDIO;
          motor_timer_start = millis();
          inStudio = 1;
      } else if (Lines.checkSensor(LEFT, RED)) {
        // Motors.hardFwdLeft();
        Motors.moveBot(CCW,CCW,12,36);
          //Serial.println("HARDLEFT");
        lastRed = LEFT;
      } else if (Lines.checkSensor(RIGHT, RED)) {
        // Motors.hardFwdRight();
        Motors.moveBot(CCW,CCW,36,12);
          //Serial.println("HARDRIGHT");
        lastRed = RIGHT;
      } else if (Lines.checkSensor(LEFT_MID, RED) && !Lines.checkSensor(RIGHT_MID, RED)) {
        // Motors.softFwdLeft();
        Motors.moveBot(CCW,CCW,20,36);
          //Serial.println("SOFTLEFT");
        lastRed = LEFT_MID;
      } else if (Lines.checkSensor(RIGHT_MID, RED) && !Lines.checkSensor(LEFT_MID, RED)) {
        // Motors.softFwdRight();
        Motors.moveBot(CCW,CCW,36,20);
          //Serial.println("SOFTRIGHT");
        lastRed = RIGHT_MID;
      } else {
        if (lastRed == LEFT) {
          // Motors.hardFwdLeft();
          Motors.moveBot(CCW,CCW,12,36);
            //Serial.println("HARDLEFT");
        } else if (lastRed == RIGHT) {
          // Motors.hardFwdRight();
          Motors.moveBot(CCW,CCW,36,12);
            //Serial.println("HARDRIGHT");
        } else if (lastRed == LEFT_MID) {
          // Motors.softFwdLeft();
          Motors.moveBot(CCW,CCW,20,36);
            //Serial.println("SOFTLEFT");
        } else if (lastRed == RIGHT_MID) {
          // Motors.softFwdRight();
          Motors.moveBot(CCW,CCW,36,20);
            //Serial.println("SOFTRIGHT");
        }
      }
      break;
    case PRESS_DISP:
      delay(PD_DURATION * 1000); // wait for press to dispense
      PressDispenser.monitorShutdown(game_start_time + 130001);
      STATE = DRIVE_BWD;
      // Motors.backward();
      Motors.moveBot(CW,CW,60,60);
      motor_timer_start = millis();
      current_millis = millis();
      break;
    case DRIVE_BWD:
      if ((current_millis-motor_timer_start) >= 375){ // TODO: adjust with speed // GAIN
        STATE = TURNING;
        motor_timer_start = millis();
        motor_timer_duration = 400; // 90-deg turn // GAIN    
        // Motors.slowLeft();
        Motors.moveBot(CW,CCW,35,35);
      }
      break;
    case DRIVE_FWD_FROM:
      // TODO: add protection from black line
      if (Lines.checkSensor(RIGHT,BLACK)){
        // stop, make a left turn, and then go straight
        Motors.idle();
        STATE = TURNING;
        motor_timer_start = millis();
        motor_timer_duration = 250; // 90-deg turn   // GAIN 
        // Motors.slowLeft();
        Motors.moveBot(CW,CCW,35,35);
        hitBlackLine = 1;
      }
      if (hitBlackLine == 1){ // TODO: adjust for speed
        STATE = SOFT_TURNING_FROM;
        // Motors.forward();
        Motors.moveBot(CCW,CCW,60,60);
        // Motors.moveBot(CCW, CCW, FAST - 10, FAST); // TODO: adjust for speed
      } else if ((current_millis-motor_timer_start) >= 1000) {
        STATE = SOFT_TURNING_FROM;
        // Motors.forward();
        Motors.moveBot(CCW,CCW,60,60);
      }
      break;
    case SOFT_TURNING_FROM:
      if (Lines.checkSensor(RIGHT,BLACK)){
        // stop, make a left turn, and then go straight
        Motors.idle();
        STATE = TURNING;
        motor_timer_start = millis();
        motor_timer_duration = 250; // 90-deg turn   // GAIN 
        // Motors.slowLeft();
        Motors.moveBot(CW,CCW,35,35);
        hitBlackLine = 1;
      }
      if (Lines.checkSensor(RIGHT_MID, RED) || Lines.checkSensor(LEFT_MID, RED) || Lines.checkSensor(LEFT, RED)) {
        if (Lines.checkSensor(LEFT, RED)) {lastRed = LEFT;}
        else if (Lines.checkSensor(LEFT_MID, RED)) {lastRed = LEFT_MID;}
        else if (Lines.checkSensor(RIGHT_MID, RED)) {lastRed = RIGHT_MID;}
        else {lastRed = RIGHT;}
        hitBlackLine = 0;
        motor_timer_start = millis();
        motor_timer_duration = 342;  // GAIN     
        STATE = TURNING;
        // Motors.slowLeft();
        Motors.moveBot(CW,CCW,35,35);
        hitRedLineOnWayBack = 1;
      }
      break;
    case INTO_STUDIO:
      if ((current_millis-motor_timer_start) >= 1500){ // TODO: adjust with speed // GAIN
        STATE = FINDING_BEACON_RESTART;
        // Motors.slowLeft();
        Motors.moveBot(CW,CCW,35,35);
      }
      break;
    case FINDING_BEACON_RESTART:
      if (Beacon.checkForFrequency(our_freq, INSTANT)){
        Motors.idle();
        STATE = RELOADING;
      }
      break;
    case RELOADING:
      // when button press to restart,
      // transition to forward state
      // move motors forward
      if (digitalRead(RESET_PIN)) { // replace with button press flag
        STATE = STUDIO_BLK;
        // Motors.forward();
        Motors.moveBot(CCW,CCW,60,60);
        LF_STATE = LINE_FOLLOW_TO;
        hitLineOnWayThere = 0;
      }
      break;
    case GAMEOVER:
      Motors.idle();
      break;
  }

  checkGlobalEvents();
}

void checkGlobalEvents(void) {
  // if (TestForKey()) RespToKey();

  if (CrowdPleaser.isRunning()) {
    CrowdPleaser.monitorShutdown(current_millis);
  }

  // if (PressDispenser.isRunning()) {
  //   PressDispenser.monitorShutdown(current_millis);
  // }

  if ((STATE != GAMEOVER) && (current_millis - game_start_time) >= 130000) {
    CrowdPleaser.start(CP_DURATION);
    STATE = GAMEOVER;
  }

}

uint8_t TestForKey(void) {
  uint8_t KeyEventOccurred;
  KeyEventOccurred = Serial.available();
  return KeyEventOccurred;
}

void RespToKey(void) {
  counter++;
  if (counter == 1) {
    Motors.forward();
  } else if (counter == 2) {
    Motors.backward();
  } else if (counter == 3) {
    Motors.fastLeft();
  } else if (counter == 4) {
    Motors.fastRight();
  } else if (counter == 5) {
    Motors.slowLeft();
  } else if (counter == 6) {
    Motors.slowRight();
  } else {
    counter = 0;
    Motors.idle();
  }
  // CrowdPleaser.start(CP_DURATION);
  // PressDispenser.start(PD_DURATION);
   //Serial.println(Serial.read());
}
