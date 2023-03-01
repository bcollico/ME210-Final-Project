#include <Arduino.h>
#include <LineFollowing.hpp>
#include <MotorControl.hpp>
#include <BeaconSensing.hpp>

#define USE_TIMER_2 true

// pin definitions
#define L_DPIN        7
#define L_EPIN        9
#define R_DPIN        12
#define R_EPIN        10
#define IR_PIN_IN     2

#include <TimerInterrupt.h>
#include <ISR_Timer.h>

int counter = 0;

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

// setup
void setup() {
  Serial.begin(9600);
  while(!Serial);

 attachInterrupt(digitalPinToInterrupt(Beacon.pin), incrementCounter, FALLING);

  // the beacon frequency estimate will update automatically at the 
  // specified frequency (estimate_freq).
  ITimer2.init();
  ITimer2.attachInterrupt(Beacon.estimate_freq, interruptHandler);
  
  Motors.idle();
}

void loop() {
  // no need to call Beacon.Update() as the Timer Interrup service
  // handles this at a rate of approximately 45 hz

  // you can update the frequency of the beacon estimate by calling
  // Beacon.updateEstimateFreq((float)10) e.g. update at 10hz

  // check if we can see the high or low freq beacons
  // optional argument to switch type of estimate: AVERAGE/INSTANT

  Lines.Update();  // call this as frequently as you want to update the averages

  // current_millis = millis();
  // if ((current_millis - timer_start) >= buffer) {
  //   timer_start = millis();
  //   // use Line.checkAnySensor and a color (RED/BLACK/WHITE) to check if
  //   // any sensor is detecting the specified color.
  //   if (Lines.checkAnySensor(BLACK)) {
  //     Serial.println("BLACK TAPE!");
  //   }

  //   // use Line.checkSensor to check if the LEFT/LEFT_MID/RIGHT_MID/RIGHT
  //   // sensors are over the specified color.
  //   if (Lines.checkSensor(RIGHT, RED)){
  //     Serial.println("TURN RIGHT!");
  //   }

  //   // use Beacon.checkForFrequency to return a boolean if we find a 
  //   // HIGH_FREQ/LOW_FREQ signal
  //   if (Beacon.checkForFrequency(HIGH_FREQ)) {
  //     Serial.println("FOUND 3333HZ.");
  //   }

  //   if (Beacon.checkForFrequency(LOW_FREQ)) {
  //     Serial.println("FOUND 909HZ.");
  //   }
  // }
  checkGlobalEvents();
}

void checkGlobalEvents(void) {
  if (TestForKey()) RespToKey();
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
  Serial.println(Serial.read());
}