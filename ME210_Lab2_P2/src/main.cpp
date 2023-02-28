#include <Arduino.h>
#include "TimerInterrupt.h" 
#include "ISR_Timer.h"

/*---------------Module Function Prototypes-----------------*/
void checkGlobalEvents(void);
unsigned char TestForKey(void);
void RespToKey(void);

/*---------------Module Variables---------------------------*/
bool toggle = HIGH;

/*----------------------Main Functions----------------------*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  analogWrite(3, map(analogRead(0),0,1023,0,255));
}

void loop() {
  checkGlobalEvents();
  analogWrite(3, map(analogRead(0),0,1023,0,255));
}

/*----------------Module Functions--------------------------*/

void checkGlobalEvents(void) {
  if (TestForKey()) RespToKey();
}

uint8_t TestForKey(void) {
  uint8_t KeyEventOccurred;
  KeyEventOccurred = Serial.available();
  return KeyEventOccurred;
}

void RespToKey(void) {
  toggle = !toggle;
  digitalWrite(5,toggle);
}