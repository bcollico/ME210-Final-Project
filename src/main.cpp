#include <Arduino.h>
#include <LineFollowing.hpp>
#include <MotorControl.hpp>
#include <BeaconSensing.hpp>

#define USE_TIMER_2 true

#include <TimerInterrupt.h>
#include <ISR_Timer.h>

// these are placeholders. need to actually specify the states
typedef enum {
  FAST_L, SLOW_L, FAST_R, SLOW_R, FWD
} States_t;

// instantiate line following class
LineFollowing Lines = LineFollowing();

// instantiate motor control class
MotorControl Motors = MotorControl();

// instantiate beacon sensing class with digital pin 2 as input
BeaconSensing Beacon = BeaconSensing((int)2, (float)90.9);

void incrementCounter(){
  Beacon.incrementCounter();
}

void setup() {
  Serial.begin(9600);
  while(!Serial);

  attachInterrupt(digitalPinToInterrupt(Beacon.pin), incrementCounter, FALLING);

  ITimer2.init();
  ITimer2.attachInterrupt(Beacon.estimate_freq, interruptHandler, Beacon);

}

void loop() {
  Lines.Update();  // call this as frequently as you want to update the averages

  // use Line.checkAnySensor and a color (RED/BLACK/WHITE) to check if
  // any sensor is detecting the specified color.
  if (Lines.checkAnySensor(BLACK)) {
    Serial.println("BLACK TAPE!");
  }

  // use Line.checkSensor to check if the LEFT/LEFT_MID/RIGHT_MID/RIGHT
  // sensors are over the specified color.
  if (Lines.checkSensor(RIGHT, RED)){
    Serial.println("TURN RIGHT!");
  }

}




