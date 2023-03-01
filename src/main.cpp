#include <Arduino.h>
#include <LineFollowing.hpp>
#include <MotorControl.hpp>
#include <BeaconSensing.hpp>
#include <CrowdPleasing.hpp>

#define USE_TIMER_2 true

// pin definitions
#define L_DPIN        7
#define L_EPIN        9
#define R_DPIN        12
#define R_EPIN        10
#define IR_PIN_IN     2
#define CP_PIN        6
#define FREQ_SWITCH_PIN 4

#include <TimerInterrupt.h>
#include <ISR_Timer.h>

// counter for respToKey
int counter = 0;

// motor timer
unsigned int motor_timer_start;
unsigned int motor_timer_duration;
unsigned int rotation_time[2] = {500, 1000}; // [ms]

/* ---------- STATES ----------*/
typedef enum {
  BOT_IDLE, FINDING_BEACON, AT_STUDIO, DRIVE_FWD, TURNING,
  PRESS_DISP, INTO_STUDIO, GAMEOVER
} States_t;

typedef enum {STUDIO_BLK, STUDIO_RED, PRESS_BLK, PRESS_RED_1, 
PRESS_RED_2, LINE_FOLLOWING}
FwdStates_t;


FwdStates_t FWD_STATE = STUDIO_RED;
States_t STATE = BOT_IDLE;

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
  
  Motors.idle();

  CrowdPleaser.start(CP_DURATION);

  pinMode(FREQ_SWITCH_PIN, INPUT);

  if (digitalRead(FREQ_SWITCH_PIN)) {
    our_freq = HIGH_FREQ;
  } else {
    our_freq = LOW_FREQ;
  }
}

void loop() {


  Lines.Update();  // call this as frequently as you want to update the averages

  // IDLE, FINDING_BEACON, INIT_ORIENTATION, AT_STUDIO, DRIVE_FWD, TURNING,
  // PRESS_DISP, INTO_STUDIO, GAMEOVER

  switch (STATE) {
    case BOT_IDLE:

      // transition to beacon finding
      Motors.slowLeft();
      STATE = FINDING_BEACON;

    case FINDING_BEACON:
      // 1. start slow rotation
      // 2. check for signal via frequency estimate
      // 3. stop and switch states when found

      if (Beacon.checkForFrequency(our_freq)){
        // transition to INIT_ORIENTATION
        Serial.println("FOUND BEACON.");
        motor_timer_start = millis();
        motor_timer_duration = rotation_time[1]; // 180-deg turn        
        STATE = TURNING;
      }

    case AT_STUDIO:
      break;
    case DRIVE_FWD:

      // STUDIO_BLK, STUDIO_RED, PRESS_BLK, PRESS_RED_1, PRESS_RED_2
      switch (FWD_STATE) {

        case STUDIO_RED:
          if (Lines.checkAnySensor(RED)) {
            Serial.println("FOUND RED TAPE.");
            Motors.idle();
            FWD_STATE = LINE_FOLLOWING;
          }

        case LINE_FOLLOWING:

          // catches for the outer sensors
          if (Lines.checkAnySensor(BLACK)) {
            Serial.println("REACHED PRESS LINE.");
            Motors.fastRight();
            motor_timer_duration = rotation_time[0];
            motor_timer_start = millis();
            FWD_STATE = PRESS_RED_1;
            STATE = TURNING;

          } else if (Lines.checkSensor(LEFT, RED)) {
            Motors.fastLeft();
            motor_timer_duration = rotation_time[0]/2;
            motor_timer_start = millis();
            STATE = TURNING;

          } else if (Lines.checkSensor(RIGHT, RED)) {
            Motors.fastRight();
            motor_timer_duration = rotation_time[0]/2;
            motor_timer_start = millis();
            STATE = TURNING;

          } else if (Lines.checkSensor(LEFT_MID, RED)) {
            Motors.slowLeft();
            motor_timer_duration = rotation_time[0]/2;
            motor_timer_start = millis();
            STATE = TURNING;

          } else if (Lines.checkSensor(RIGHT_MID, RED)) {
            Motors.slowRight();
            motor_timer_duration = rotation_time[0]/2;
            motor_timer_start = millis();
            STATE = TURNING;

          }

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
      break;
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
  Serial.println(Serial.read());
}
