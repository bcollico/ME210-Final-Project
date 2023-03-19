#include <Arduino.h>
#include <LineFollowing.hpp>
#include <MotorControl.hpp>
#include <BeaconSensing.hpp>
#include <CrowdPleasing.hpp>
#include <PressDispensing.hpp>

#define USE_TIMER_2 true
<<<<<<< Updated upstream

// pin definitions
#define L_DPIN        9
#define L_EPIN        7
#define R_DPIN        10
#define R_EPIN        12
#define IR_PIN_IN     2
#define CP_PIN        6
#define PD_PIN        8
#define FREQ_SWITCH_PIN 4
=======
>>>>>>> Stashed changes

#include <TimerInterrupt.h>
#include <ISR_Timer.h>

<<<<<<< Updated upstream
// counter for respToKey
int counter = 0;
=======
// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire,0x68);
>>>>>>> Stashed changes

// motor timer
long int motor_timer_start;
long int motor_timer_duration;
int rotation_time[2] = {1100, 1800}; // [ms]
int lastRed = -1;

// game timer
long int game_start_time = millis();

// store beacon frequency
Freqs_t our_freq;


/* ---------- STATES ----------*/
typedef enum {
<<<<<<< Updated upstream
  BOT_IDLE, FINDING_BEACON, DRIVE_FWD, TURNING,
  PRESS_DISP, GAMEOVER
=======
  BOT_IDLE, FINDING_BEACON, TURNING,
  PRESS_DISP, GAMEOVER, DRIVE_BWD, 
  LINE_FOLLOWING, INTO_STUDIO, RELOADING, STUDIO_BLK,
  DRIVE_FWD_TO, DRIVE_FWD_FROM, FINDING_BEACON_RESTART
>>>>>>> Stashed changes
} States_t;

typedef enum {STUDIO_BLK, PRESS_RED_1, 
PRESS_RED_2, LINE_FOLLOWING_LEAVE, LINE_FOLLOWING_RETURN}
FwdStates_t;


FwdStates_t FWD_STATE = STUDIO_BLK;
States_t STATE = BOT_IDLE;

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

<<<<<<< Updated upstream
// setup
=======
void resetToBotIdle() {
  STATE = BOT_IDLE;
}

>>>>>>> Stashed changes
void setup() {
  Serial.begin(9600);
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
<<<<<<< Updated upstream
    our_freq = LOW_FREQ; // TODO: switch to LOW/HIGH_FREQ
  }

=======
    our_freq = HIGH_FREQ;
  }

  pinMode(FREQ_SWITCH_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(FREQ_SWITCH_PIN), resetToBotIdle, CHANGE);

>>>>>>> Stashed changes
  // Lines.calibrate_sensors();

  // delay(20000);

  // STATE = GAMEOVER;
}

void loop() {

<<<<<<< Updated upstream
  // Serial.println(STATE);

  current_millis = millis();
  Lines.Update();  // call this as frequently as you want to update the averages

  // IDLE, FINDING_BEACON, INIT_ORIENTATION, AT_STUDIO, DRIVE_FWD, TURNING,
  // PRESS_DISP, INTO_STUDIO, GAMEOVER

  switch (STATE) {
    case BOT_IDLE:

      // transition to beacon finding
      Motors.slowRight();
      STATE = FINDING_BEACON;
=======
  current_millis = millis();
  Lines.Update();

  switch (STATE) {
    case BOT_IDLE:
      STATE = FINDING_BEACON;
      Motors.slowLeft();
>>>>>>> Stashed changes
      break;

    case FINDING_BEACON:
      if (Beacon.checkForFrequency(our_freq, INSTANT)){
        Motors.idle();
<<<<<<< Updated upstream
        // delay(3000);
        motor_timer_start = millis();
        motor_timer_duration = rotation_time[1]; // 180-deg turn        
        STATE = TURNING;
        FWD_STATE = STUDIO_BLK;
        Motors.slowRight();
        break;
      }
=======
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
>>>>>>> Stashed changes
      break;

    case TURNING:

      if ((current_millis - motor_timer_start) >= motor_timer_duration){
        Motors.idle();
<<<<<<< Updated upstream
        STATE = DRIVE_FWD;
        Motors.forward();      }
      break;

    case DRIVE_FWD:

      switch (FWD_STATE) {

        case STUDIO_BLK:
          if (Lines.checkAnySensor(BLACK)) {
            Motors.forward();
            // ----- CHANGE STATE ------ //
            FWD_STATE = LINE_FOLLOWING_LEAVE;
            // ------------------------- //
            motor_timer_start = millis();
            current_millis = millis();

            //TODO: REMOVE THIS BLOCKING CODE
            while ((current_millis-motor_timer_start < 500)){
              current_millis = millis();
            }
            break;
          }
          break;

        case LINE_FOLLOWING_LEAVE:

          if (Lines.checkAnySensor(BLACK)) {
            // Serial.println("REACHED PRESS LINE.");
            Motors.idle();
            delay(250);
            Motors.backward();
            motor_timer_start = millis();
            // // TODO: Remove blocking code
            while((current_millis - motor_timer_start) < 750) {
              current_millis = millis();
            }
            Motors.slowRight();
            motor_timer_start = millis();
            motor_timer_duration = rotation_time[0]; // 90-deg turn 

            // ----- CHANGE STATE ------ // 
            STATE = TURNING;
            FWD_STATE = PRESS_RED_1;
            black_tape_timer = 1500+motor_timer_duration;
            // ------------------------- //

            lastRed = -1;
            black_tape_start = millis();

            break;
          } else if (Lines.checkSensor(LEFT, RED)) {
            Motors.hardFwdLeft(25);
             //Serial.println("HARDLEFT");
            lastRed = LEFT;
            break;
          } else if (Lines.checkSensor(RIGHT, RED)) {
            Motors.hardFwdRight(25);
             //Serial.println("HARDRIGHT");
            lastRed = RIGHT;
            break;
          } else if (Lines.checkSensor(LEFT_MID, RED) && !Lines.checkSensor(RIGHT_MID, RED)) {
            Motors.softFwdLeft(15);
             //Serial.println("SOFTLEFT");
            lastRed = LEFT_MID;
            break;
          } else if (Lines.checkSensor(RIGHT_MID, RED) && !Lines.checkSensor(LEFT_MID, RED)) {
            Motors.softFwdRight(15);
             //Serial.println("SOFTRIGHT");
            lastRed = RIGHT_MID;
            break;
          } else {
            if (lastRed == LEFT) {
              Motors.hardFwdLeft(25);
               //Serial.println("HARDLEFT");
            } else if (lastRed == RIGHT) {
              Motors.hardFwdRight(25);
               //Serial.println("HARDRIGHT");
            } else if (lastRed == LEFT_MID) {
              Motors.softFwdLeft(15);
               //Serial.println("SOFTLEFT");
            } else if (lastRed == RIGHT_MID) {
              Motors.softFwdRight(15);
               //Serial.println("SOFTRIGHT");
            }
          }

          break;

        case PRESS_RED_1:

          if (((current_millis-black_tape_start) > (black_tape_timer)) && 
          (change_to_press || 
          (Lines.checkSensor(RIGHT_MID,RED) || Lines.checkSensor(RIGHT,RED)))) {

            // ----- CHANGE STATE ------ // 
            if (change_to_press){
              Motors.idle();
              motor_timer_start = millis();
              // // TODO: Remove blocking code
              while((current_millis - motor_timer_start) < 500) {
                current_millis = millis();
              }

              STATE = PRESS_DISP;
              PressDispenser.start(PD_DURATION);

              FWD_STATE = PRESS_RED_2;
              // skip the turning stage for the first press dispensing
              motor_timer_duration = 0;
              lastRed = -1;
              black_tape_timer = 2000;
              change_to_press = false;
            // ------------------------- //
            } else {
              black_tape_start = millis();
              black_tape_timer = 250;
              change_to_press = true;
            }

            break;
          } else if (Lines.checkSensor(LEFT, BLACK)){
            lastRed = LEFT;
            Motors.softFwdRight(35);
             //Serial.println("SOFTRIGHT");
          } else if (!Lines.checkSensor(LEFT, BLACK)) {
            lastRed = RIGHT;
            Motors.softFwdLeft(20);
             //Serial.println("SOFTLEFT");
          } else {
            if (lastRed == LEFT) {
              Motors.softFwdRight(35);
               //Serial.println("SOFTRIGHT");
            } else if (lastRed == RIGHT) {
              Motors.softFwdLeft(20);
               //Serial.println("SOFTLEFT");
            }
          }

          break;
        
        case PRESS_RED_2:

          if (((current_millis-black_tape_start) > (black_tape_timer)) && 
          (change_to_press || 
          (Lines.checkSensor(RIGHT_MID,RED) || Lines.checkSensor(RIGHT,RED)))) {
            
            if (change_to_press) {
              Motors.idle();
              motor_timer_start = millis();
              while((current_millis - motor_timer_start) < 500) {
                current_millis = millis();
              }

              // ----- CHANGE STATE ------ // 
              STATE = PRESS_DISP;
              PressDispenser.start(PD_DURATION);
              FWD_STATE = LINE_FOLLOWING_RETURN;
              motor_timer_duration = rotation_time[0]; // 90 DEG
              lastRed = -1;
              // ------------------------- //
            } else {
              black_tape_start = millis();
              black_tape_timer = 250;
              change_to_press = true;
            }
            break;
          } else if (Lines.checkSensor(LEFT, BLACK)){
            lastRed = LEFT;
            Motors.softFwdRight(30);
             //Serial.println("SOFTRIGHT");
             break;
          } else if (!Lines.checkSensor(LEFT, BLACK)) {
            lastRed = RIGHT;
            Motors.softFwdLeft(20);
             //Serial.println("SOFTLEFT");
             break;
          } else {
            if (lastRed == LEFT) {
              Motors.softFwdRight(30);
               //Serial.println("SOFTRIGHT");
            } else if (lastRed == RIGHT) {
              Motors.softFwdLeft(20);
               //Serial.println("SOFTLEFT");
               break;
            }
          }

          break;

        case LINE_FOLLOWING_RETURN:

          if (Lines.checkAnySensor(BLACK)) {
             //Serial.println("REACHED STUDIO LINE.");
             Motors.forward();
            // ----- CHANGE STATE ------ // 
            STATE = GAMEOVER;
            
            // ------------------------- //
            motor_timer_start = millis();
            current_millis = millis();

            //TODO: REMOVE THIS BLOCKING CODE
            while ((current_millis-motor_timer_start) < 750){
              current_millis = millis();
            }
            CrowdPleaser.start(CP_DURATION);
            break;
          } else if (Lines.checkSensor(LEFT, RED)) {
            Motors.hardFwdLeft();
             //Serial.println("HARDLEFT");
            lastRed = LEFT;
            break;
          } else if (Lines.checkSensor(RIGHT, RED)) {
            Motors.hardFwdRight();
             //Serial.println("HARDRIGHT");
            lastRed = RIGHT;
            break;
          } else if (Lines.checkSensor(LEFT_MID, RED) && !Lines.checkSensor(RIGHT_MID, RED)) {
            Motors.softFwdLeft();
             //Serial.println("SOFTLEFT");
            lastRed = LEFT_MID;
            break;
          } else if (Lines.checkSensor(RIGHT_MID, RED) && !Lines.checkSensor(LEFT_MID, RED)) {
            Motors.softFwdRight();
             //Serial.println("SOFTRIGHT");
            lastRed = RIGHT_MID;
            break;
          } else {
            if (lastRed == LEFT) {
              Motors.hardFwdLeft();
               //Serial.println("HARDLEFT");
            } else if (lastRed == RIGHT) {
              Motors.hardFwdRight();
               //Serial.println("HARDRIGHT");
            } else if (lastRed == LEFT_MID) {
              Motors.softFwdLeft();
               //Serial.println("SOFTLEFT");
            } else if (lastRed == RIGHT_MID) {
              Motors.softFwdRight();
               //Serial.println("SOFTRIGHT");
            }
            break;
          }

        break;
=======
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
>>>>>>> Stashed changes
      }
      break;

    case PRESS_DISP:
<<<<<<< Updated upstream
      if (!PressDispenser.isRunning()){
=======
      if (!PressDispenser.isRunning()) {
        STATE = DRIVE_BWD;
        Motors.backward();
        motor_timer_start = millis();
        current_millis = millis();
      }
      break;
    case DRIVE_BWD:
      if ((current_millis-motor_timer_start) >= 375){ // GAIN
>>>>>>> Stashed changes
        STATE = TURNING;
        Motors.idle(); // added for testing only
        motor_timer_start = millis();
<<<<<<< Updated upstream
        Motors.slowRight();

        black_tape_start = millis();

        // the motor timer gets set by the PRESS_RED_1/2 states
        // motor_timer_duration = 0; 
=======
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
>>>>>>> Stashed changes
      }
      break;

    case GAMEOVER:
      Motors.idle();
      break;
  }

  // current_millis = millis();
  // if ((current_millis - timer_start) >= buffer) {
  //   for (int i = 0; i < 4; i++) {
  //     Serial.print(Lines.meas_vals[i][Lines.newest_idx]);
  //     Serial.print(", ");
  //   }
  //   Serial.println("");
  //   timer_start = millis();
  // }

  checkGlobalEvents();
}

void checkGlobalEvents(void) {
  if (CrowdPleaser.isRunning()) {
    CrowdPleaser.monitorShutdown(current_millis);
  }

  if (PressDispenser.isRunning()) {
    PressDispenser.monitorShutdown(current_millis);
<<<<<<< Updated upstream
=======
  }

  if ((STATE != GAMEOVER) && (current_millis - game_start_time) >= 130000) {
    CrowdPleaser.start(CP_DURATION);
    STATE = GAMEOVER;
>>>>>>> Stashed changes
  }

  // if ((STATE != GAMEOVER) && (current_millis - game_start_time) >= 130000) {
  //   CrowdPleaser.start(CP_DURATION);
  //   STATE = GAMEOVER;
  // }

  // Serial.print(Motors.L_speed);
  // Serial.print(", ");
  // Serial.println(Motors.R_speed);
}
<<<<<<< Updated upstream

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
  CrowdPleaser.start(CP_DURATION);
  PressDispenser.start(PD_DURATION);
   //Serial.println(Serial.read());
}
=======
>>>>>>> Stashed changes
