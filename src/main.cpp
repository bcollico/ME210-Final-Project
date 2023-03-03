#include <Arduino.h>
#include <LineFollowing.hpp>
#include <MotorControl.hpp>
#include <BeaconSensing.hpp>
#include <CrowdPleasing.hpp>
#include <PressDispensing.hpp>

#define USE_TIMER_2 true

// pin definitions
#define L_DPIN        9
#define L_EPIN        7
#define R_DPIN        10
#define R_EPIN        12
#define IR_PIN_IN     2
#define CP_PIN        6
#define PD_PIN        8
#define FREQ_SWITCH_PIN 4

#include <TimerInterrupt.h>
#include <ISR_Timer.h>

// counter for respToKey
int counter = 0;

// motor timer
unsigned int motor_timer_start;
unsigned int motor_timer_duration;
unsigned int rotation_time[2] = {100, 200}; // [ms]

/* ---------- STATES ----------*/
typedef enum {
  BOT_IDLE, FINDING_BEACON, AT_STUDIO, DRIVE_FWD, TURNING,
  PRESS_DISP, INTO_STUDIO, GAMEOVER
} States_t;

typedef enum {STUDIO_BLK, STUDIO_RED, PRESS_BLK, PRESS_RED_1, 
PRESS_RED_2, LINE_FOLLOWING}
FwdStates_t;


FwdStates_t FWD_STATE = LINE_FOLLOWING;
States_t STATE = DRIVE_FWD;

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
unsigned long int current_millis = millis();
unsigned long int timer_start = millis();
unsigned int buffer = 200;

// timer interrupt functions for IR beacon sensing
void incrementCounter(){
  Beacon.incrementCounter();
}

void interruptHandler(){
  Beacon.Update();
}

Freqs_t our_freq;

// setup
void setup() {
  Serial.begin(9600);
  while(!Serial);

  int initTime = millis();

  attachInterrupt(digitalPinToInterrupt(Beacon.pin), incrementCounter, FALLING);

  // the beacon frequency estimate will update automatically at the 
  // specified frequency (estimate_freq).
  ITimer2.init();
  ITimer2.attachInterrupt(Beacon.estimate_freq, interruptHandler);

  CrowdPleaser.start(CP_DURATION);

  pinMode(FREQ_SWITCH_PIN, INPUT);

  if (digitalRead(FREQ_SWITCH_PIN)) {
    our_freq = LOW_FREQ; // TODO: switch to HIGH_FREQ
  } else {
    our_freq = LOW_FREQ;
  }

  Motors.forward(); // for line following testing only
}

void loop() {

  current_millis = millis();
  Lines.Update();  // call this as frequently as you want to update the averages

  // IDLE, FINDING_BEACON, INIT_ORIENTATION, AT_STUDIO, DRIVE_FWD, TURNING,
  // PRESS_DISP, INTO_STUDIO, GAMEOVER

  switch (STATE) {
    case BOT_IDLE:

      // transition to beacon finding
      Motors.slowLeft();
      // if (Beacon.checkForFrequency(our_freq)) {
      //   Serial.println("It happened.");
      // }
      Serial.println(Beacon.avg_freq);
      // STATE = FINDING_BEACON;
      break;

    case FINDING_BEACON:
      // 1. start slow rotation
      // 2. check for signal via frequency estimate
      // 3. stop and switch states when found

      if (Beacon.checkForFrequency(our_freq)){
        // transition to INIT_ORIENTATION
        Motors.idle();
        delay(3000);
        Serial.println("FOUND BEACON.");
        motor_timer_start = millis();
        motor_timer_duration = rotation_time[1]; // 180-deg turn        
        STATE = TURNING;
        Motors.slowLeft();
      }

    case AT_STUDIO:
      break;
    case DRIVE_FWD:

      // STUDIO_BLK, STUDIO_RED, PRESS_BLK, PRESS_RED_1, PRESS_RED_2
      switch (FWD_STATE) {

        case STUDIO_RED:
          if (Lines.checkAnySensor(RED)) {
            Serial.println("FOUND RED TAPE.");
            FWD_STATE = LINE_FOLLOWING;
          }

        case LINE_FOLLOWING:

          // catches for the outer sensors
          // if (Lines.checkAnySensor(BLACK)) {
          //   Serial.println("REACHED PRESS LINE.");
          //   Motors.idle(); // added for testing only
          //   delay(10000); // added for testing only
          //   Motors.fastRight();
          //   motor_timer_duration = rotation_time[0];
          //   motor_timer_start = millis();
          //   FWD_STATE = PRESS_RED_1;
          //   STATE = TURNING;

          Motors.idle();

          // if ((current_millis-timer_start) >= 200){
          timer_start = millis();
          Serial.print(Lines.vals[0][Lines.newest_idx]);
          Serial.print(", ");
          Serial.print(Lines.vals[1][Lines.newest_idx]);
          Serial.print(", ");
          Serial.print(Lines.vals[2][Lines.newest_idx]);
          Serial.print(", ");
          Serial.println(Lines.vals[3][Lines.newest_idx]);
          // }


          // if (Lines.checkSensor(LEFT, RED)) {
          //   Motors.hardFwdLeft();

          // } else if (Lines.checkSensor(RIGHT, RED)) {
          //   Motors.hardFwdRight();

          // } else if (Lines.checkSensor(LEFT_MID, RED) && !Lines.checkSensor(RIGHT_MID, RED)) {
          //   Motors.softFwdLeft();

          // } else if (Lines.checkSensor(RIGHT_MID, RED) && !Lines.checkSensor(LEFT_MID, RED)) {
          //   Motors.softFwdRight();

          // }

        case STUDIO_BLK:
          break;

        case PRESS_BLK: 
          break;
        case PRESS_RED_1:
          Serial.println("STOP.");
          Motors.idle();

          // if (Lines.checkAnySensor(RED)){
          //   if (BAD_PRESS) {
          //   Motors.idle();
          //   Serial.println("DROP PRESS.");

          //   motor_timer_start = millis();
          //   motor_timer_duration = 3000;
          //   STATE = PRESS_DISP;
          //   } else {
          //       Motors.idle();
          //   Serial.println("DROP PRESS.");

          //   motor_timer_start = millis();
          //   motor_timer_duration = 3000;
          //   FWD_STATE = PRESS_RED_2;
          //   }
          // }
        
        case PRESS_RED_2:

          if (Lines.checkAnySensor(RED)){
            Motors.idle();
            Serial.println("DROP PRESS.");

            motor_timer_start = millis();
            motor_timer_duration = 3000;
            FWD_STATE = PRESS_RED_2;
          }
      }

    case TURNING:

      if ((current_millis - motor_timer_start) >= motor_timer_duration){
        STATE = DRIVE_FWD;
        Motors.forward();
      }


    case INTO_STUDIO:
      break;
    case PRESS_DISP:
      PressDispenser.start(PD_DURATION);
    case GAMEOVER:
      break;
  }

  current_millis = millis();
  if ((current_millis - timer_start) >= buffer) {
    timer_start = millis();
  }
  checkGlobalEvents();
}

void checkGlobalEvents(void) {
  if (TestForKey()) RespToKey();
  if (CrowdPleaser.isRunning()) {
    CrowdPleaser.monitorShutdown(current_millis);
  }
  if (PressDispenser.isRunning()) {
    PressDispenser.monitorShutdown(current_millis);
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
  CrowdPleaser.start(CP_DURATION);
  PressDispenser.start(PD_DURATION);
  Serial.println(Serial.read());
}
