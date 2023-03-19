#include <Arduino.h>
#include <LineFollowing.hpp>
#include <MotorControl.hpp>
#include <BeaconSensing.hpp>
#include <CrowdPleasing.hpp>
#include <PressDispensing.hpp>
#include <Parameters.hpp>

#define USE_TIMER_2 true

#include <TimerInterrupt.h>
#include <ISR_Timer.h>

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

// store beacon frequency
Freqs_t our_freq;


/* ---------- STATES ----------*/
typedef enum {
  BOT_IDLE, FINDING_BEACON, TURNING,
  PRESS_DISP, GAMEOVER, DRIVE_BWD, 
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

// timer interrupt functions for IR beacon sensing
void incrementCounter(){
  Beacon.incrementCounter();
}

void interruptHandler(){
  Beacon.Update();
}

void resetToBotIdle() {
  STATE = BOT_IDLE;
}

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
    our_freq = HIGH_FREQ;
  }

  pinMode(FREQ_SWITCH_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(FREQ_SWITCH_PIN), resetToBotIdle, CHANGE);

  // Lines.calibrate_sensors();

}

void loop() {

  current_millis = millis();
  Lines.Update();

  switch (STATE) {
    case BOT_IDLE:
      STATE = FINDING_BEACON;
      Motors.slowLeft();
      break;
    case FINDING_BEACON:
      if (Beacon.checkForFrequency(our_freq, INSTANT)){
        Motors.idle();
        STATE = STUDIO_BLK;
        Motors.fastForward();
        break;
      }
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
        if (LF_STATE == LINE_FOLLOW_TO && hitLineOnWayThere == 1) {
          STATE = LINE_FOLLOWING;
          Motors.slowForward();
        }
        else if (LF_STATE == LINE_FOLLOW_TO){
          STATE = DRIVE_FWD_TO;
          Motors.fastForward();
          hitLineOnWayThere = 1;
        } else if (LF_STATE == LINE_FOLLOW_FROM && inStudio == 1){
          Motors.idle();
          STATE = RELOADING;
          LF_STATE = LINE_FOLLOW_TO;
        } else if (LF_STATE == LINE_FOLLOW_FROM && inStudio == 0 && hitRedLineOnWayBack == 1) {
          STATE = LINE_FOLLOWING;
          Motors.slowForward();
          hitRedLineOnWayBack = 0;
        } else {
          Motors.fastForward();
          motor_timer_start = millis();
          STATE = DRIVE_FWD_FROM;
        }
      }
      break;
    case DRIVE_FWD_TO:
      if (hitLineOnWayThere == 0 && Lines.checkAnySensor(RED)){ // GAIN
        lastRed = 10; // to null out any lastRed behavior after turn
        Motors.idle();
        STATE = TURNING;
        motor_timer_start = millis();
        motor_timer_duration = 342; // GAIN  
        Motors.fastLeft();
      } else if (Lines.checkSensor(LEFT, RED)) { // GAIN
        lastRed = 10; // to null out any lastRed behavior after turn
        motor_timer_start = millis();
        motor_timer_duration = 342; // GAIN     
        STATE = TURNING;
        Motors.fastLeft();
        }
        if (current_millis-motor_timer_start >= 1000) {
          hitLineOnWayThere = 1;
        }
      break;
    case LINE_FOLLOWING:
      if (Lines.checkAnySensor(BLACK) && LF_STATE == LINE_FOLLOW_TO) {
        Motors.idle();
        STATE = PRESS_DISP;
        PressDispenser.start(PD_DURATION);
        LF_STATE = LINE_FOLLOW_FROM;
      } else if (Lines.checkAnySensor(BLACK) && LF_STATE == LINE_FOLLOW_FROM) {
          Motors.moveBot(CCW, CCW, FAST - 8, FAST); // GAIN
          STATE = INTO_STUDIO;
          motor_timer_start = millis();
          inStudio = 1;
      } else if (Lines.checkSensor(LEFT, RED)) {
        Motors.hardFwdLeft();
        lastRed = LEFT;
      } else if (Lines.checkSensor(RIGHT, RED)) {
        Motors.hardFwdRight();
        lastRed = RIGHT;
      } else if (Lines.checkSensor(LEFT_MID, RED) && !Lines.checkSensor(RIGHT_MID, RED)) {
        Motors.softFwdLeft();
        lastRed = LEFT_MID;
      } else if (Lines.checkSensor(RIGHT_MID, RED) && !Lines.checkSensor(LEFT_MID, RED)) {
        Motors.softFwdRight();
        lastRed = RIGHT_MID;
      } else {
        if (lastRed == LEFT) {
          Motors.hardFwdLeft();
        } else if (lastRed == RIGHT) {
          Motors.hardFwdRight();
        } else if (lastRed == LEFT_MID) {
          Motors.softFwdLeft();
        } else if (lastRed == RIGHT_MID) {
          Motors.softFwdRight();
        }
      }
      break;
    case PRESS_DISP:
      if (!PressDispenser.isRunning()) {
        STATE = DRIVE_BWD;
        Motors.backward();
        motor_timer_start = millis();
        current_millis = millis();
      }
      break;
    case DRIVE_BWD:
      if ((current_millis-motor_timer_start) >= 375){ // GAIN
        STATE = TURNING;
        motor_timer_start = millis();
        motor_timer_duration = 350; // GAIN    
        Motors.fastLeft();
      }
      break;
    case DRIVE_FWD_FROM:
      if (Lines.checkSensor(RIGHT,BLACK)){
        Motors.idle();
        STATE = TURNING;
        motor_timer_start = millis();
        motor_timer_duration = 150; // GAIN 
        Motors.fastLeft();
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
        Motors.fastLeft();
        hitRedLineOnWayBack = 1;
      }
      break;
    case INTO_STUDIO:
      if ((current_millis-motor_timer_start) >= 1500){ // GAIN
        STATE = FINDING_BEACON_RESTART;
        Motors.fastLeft();
      }
      break;
    case FINDING_BEACON_RESTART:
      if (Beacon.checkForFrequency(our_freq, INSTANT)){
        Motors.idle();
        STATE = RELOADING;
      }
      break;
    case RELOADING:
      if (digitalRead(RESET_PIN)) {
        STATE = STUDIO_BLK;
        Motors.fastForward();
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
  if (CrowdPleaser.isRunning()) {
    CrowdPleaser.monitorShutdown(current_millis);
  }

  if (PressDispenser.isRunning()) {
    PressDispenser.monitorShutdown(current_millis);
  }

  if ((STATE != GAMEOVER) && (current_millis - game_start_time) >= 130000) {
    CrowdPleaser.start(CP_DURATION);
    STATE = GAMEOVER;
  }

}
