#include <Arduino.h>

#define USE_TIMER_1 true
#define USE_TIMER_2 true

#include "TimerInterrupt.h"
#include <ISR_Timer.h>

#define TIMER_FREQ_HZ_1 909.0
#define TIMER_FREQ_HZ_2 3333.0 
#define PIN_OUT_1 3
#define PIN_OUT_2 11
#define PIN_IN_1 2 
#define PIN_IN_2 7
#define COUNTING_INTERVAL 200.0

unsigned long currentMillis = millis();
unsigned long timerStart = millis()-100;
volatile int counter = 0;
float estimated_frequency;


void TogglePin(int pin_out){
  digitalWrite(pin_out, !digitalRead(pin_out));
}

void CountFallingEdges(){
  counter++;
}

void setup() {
  Serial.begin(9600);
  while(!Serial);
  pinMode(PIN_OUT_1, OUTPUT);
  pinMode(PIN_OUT_2, OUTPUT);
  pinMode(PIN_IN_1, INPUT);
  // pinMode(PIN_IN_2, INPUT);
  ITimer1.init();
  ITimer2.init();
  ITimer1.attachInterrupt(TIMER_FREQ_HZ_1*2, TogglePin, PIN_OUT_1);
  ITimer2.attachInterrupt(TIMER_FREQ_HZ_2*2, TogglePin, PIN_OUT_2);

  attachInterrupt(digitalPinToInterrupt(PIN_IN_1), CountFallingEdges, FALLING);
}

void loop() {

  currentMillis = millis();

  if ((currentMillis - timerStart) >= COUNTING_INTERVAL){
    estimated_frequency = (counter / (COUNTING_INTERVAL/1000));

    counter = 0;
    timerStart = millis();
    if ((estimated_frequency > 2700) & (estimated_frequency < 3500)) {
      Serial.println("High Freq Detected.");
    } else if ((estimated_frequency > 700) & (estimated_frequency < 1200)) {
      Serial.println("Low Freq Detected.");
    } else {
      Serial.println(estimated_frequency);
    }
    // Serial.println(estimated_frequency);    
  }
}
