#include <Arduino.h>
#include <MPU9250.h>
#include <LineFollowing.hpp>
#include <MotorControl.hpp>
#include <BeaconSensing.hpp>
#include <CrowdPleasing.hpp>
#include <PressDispensing.hpp>

#define USE_TIMER_2 true
// #define USE_TIMER_1 true
#define IMU_FREQ_HZ 100

// pin definitions
#define L_DPIN        9
#define L_EPIN        7
#define R_DPIN        10
#define R_EPIN        12
#define IR_PIN_IN     2
#define CP_PIN        6
#define PD_PIN        8
#define FREQ_SWITCH_PIN 4

#define D90 PI/2
#define D180 PI

#include <TimerInterrupt.h>
#include <ISR_Timer.h>

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire,0x68);

// counter for respToKey
int counter = 0;

// motor timer
long int motor_timer_start;
long int motor_timer_duration;
int rotation_time[2] = {1100, 2500}; //{1100, 1800}; // [ms]
int lastRed = -1;

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
  BOT_IDLE, FINDING_BEACON, DRIVE_FWD, TURNING,
  PRESS_DISP, GAMEOVER
} States_t;

typedef enum {STUDIO_BLK, PRESS_RED_1, 
PRESS_RED_2, LINE_FOLLOWING_LEAVE, LINE_FOLLOWING_RETURN}
FwdStates_t;


FwdStates_t FWD_STATE = STUDIO_BLK;
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

float yaw = 0.0;
float vel = 0.0;
float pos = 0.0;
float T_imu = 1./IMU_FREQ_HZ;

float const PIPI = 2*PI;

void integrate_imu(){
  yaw = yaw + T_imu * IMU.getGyroZ_rads();
  // pos = pos + vel*T_imu + 0.5*T_imu*T_imu*IMU.getAccelY_mss();
  // vel = vel + T_imu * IMU.getAccelY_mss();
  pos = pos + T_imu * vel;
  vel = vel + T_imu * IMU.getAccelY_mss();
}


int control_stage = 0;

// setup
void setup() {
  Serial.begin(115200);
  while(!Serial);

  game_start_time = millis();

  attachInterrupt(digitalPinToInterrupt(Beacon.pin), incrementCounter, FALLING);

  // the beacon frequency estimate will update automatically at the 
  // specified frequency (estimate_freq).
  // ITimer2.init();
  // ITimer2.attachInterrupt(Beacon.estimate_freq, interruptHandler);

  CrowdPleaser.start(CP_DURATION);

  pinMode(FREQ_SWITCH_PIN, INPUT);

  if (digitalRead(FREQ_SWITCH_PIN)) {
    our_freq = LOW_FREQ;
  } else {
    our_freq = LOW_FREQ; // TODO: switch to LOW/HIGH_FREQ
  }

  // setup IMU
  int status = IMU.begin();
  if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while(1) {}
  }

  // setting the accelerometer full scale range to +/-8G 
  IMU.setAccelRange(MPU9250::ACCEL_RANGE_2G);
  // setting the gyroscope full scale range to +/-500 deg/s
  IMU.setGyroRange(MPU9250::GYRO_RANGE_500DPS);
  // setting DLPF bandwidth to 20 Hz
  IMU.setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_5HZ);
  // setting SRD to 19 for a 50 Hz update rate
  IMU.setSrd(10);

  // IMU.calibrateAccel();
  // IMU.calibrateGyro();

  // Serial.println(IMU.getAccelBiasY_mss());
  // Serial.println(IMU.getGyroBiasZ_rads(),10);

  IMU.setGyroBiasZ_rads(0.0133160066); //0.0133160066 //0.01340921870.0140217552
  // IMU.setAccelCalY(-6, 1.0);
  
  ITimer2.init();
  ITimer2.attachInterrupt(IMU_FREQ_HZ, integrate_imu);

  Serial.println("HERE");

  // set proportional gain to 1.25 and motor frequency to 20 HZ
  Motors.setControlParams(2.0, 1.0, 25.0);

  // Lines.calibrate_sensors();

  // delay(20000);

  // STATE = GAMEOVER;
}

void loop() {

  // Serial.println(STATE);

  current_millis = millis();
  IMU.readSensor();
  Lines.Update();  // call this as frequently as you want to update the averages

  if ((current_millis - timer_start) > 200) {
    timer_start = millis();
    Serial.print(yaw*180/PI, 6);
    Serial.print(", ");
    Serial.println(pos, 6);
  }

  switch (STATE) {
    case BOT_IDLE:

      // transition to beacon finding
      // Motors.slowRight();
      STATE = FINDING_BEACON;

      // switch (control_stage) {
      //   case 0:
      //     yaw = 0.0;
      //     Motors.StartHeadingControl(D180);
      //     control_stage = 1;
      //     break;
      //   case 1:
      //     yaw = 0.0;
      //     Motors.StartHeadingControl(-D180);
      //     control_stage = 0;
      //     break;
      // }
      // break;

      yaw = 0.0;
      Motors.StartHeadingControl(-D180);

      break;

    case FINDING_BEACON:
      // display the data

      // if (!Motors.HeadingControlActive()) {
      //   STATE = BOT_IDLE;
      //   delay(1000);
      // }

      break;

      if ((false) && Beacon.checkForFrequency(our_freq, INSTANT)){
        Motors.idle();
        // delay(3000);
        motor_timer_start = millis();
        motor_timer_duration = rotation_time[1]; // 180-deg turn        
        STATE = TURNING;
        FWD_STATE = STUDIO_BLK;
        Motors.slowRight();
        break;
      }
      break;

    case TURNING:

      if ((current_millis - motor_timer_start) >= motor_timer_duration){
        Motors.idle();
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
      }
      break;

    case PRESS_DISP:
      if (!PressDispenser.isRunning()){
        STATE = TURNING;
        Motors.idle(); // added for testing only
        motor_timer_start = millis();
        Motors.slowRight();

        black_tape_start = millis();

        // the motor timer gets set by the PRESS_RED_1/2 states
        // motor_timer_duration = 0; 
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

  if (Motors.HeadingControlActive()) {
    // check if we are currently controlling the motors
    Motors.ControlHeading(yaw, current_millis);
  }

  if (Motors.PositionControlActive()) {
    Motors.ControlPosition(pos, current_millis);
  }

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
