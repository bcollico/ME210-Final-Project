#include <Arduino.h>

#define ANALOG_IN0 A0
#define ANALOG_IN1 A1
#define ANALOG_IN2 A2
#define ANALOG_IN3 A3
#define AVG_WINDOW 100.0
// #define ANALOG_IN4 A4


// accumulates analog signal values from tape sensors

const uint8_t pins[] = {ANALOG_IN0, ANALOG_IN1, ANALOG_IN2, ANALOG_IN3};

unsigned long int counters[] = {0,0,0,0};

unsigned long int avgs[] = {0,0,0,0};

int cnt = 0;
// int tape_4;

int wht_tape = 400;
int red_tape = 250;
int blk_tape = 100;

unsigned long currentMillis = millis();
unsigned long timerStart = millis()-100;

typedef enum {
  FAST_L, SLOW_L, FAST_R, SLOW_R, FWD
} States_t;

States_t STATE = FWD;

bool checkRedWhite(uint8_t pin){
  return (pin >= red_tape) & (pin < wht_tape);
}

bool checkBlk(){
  // need to handle outliers / moving average
  for (int i=0; i++; i<5) {
    if (avgs[i] <= blk_tape) {
      return true;
    }
  }

  return false;
}

void setup() {
  Serial.begin(9600);
  while(!Serial);

  pinMode(ANALOG_IN0, INPUT);
  pinMode(ANALOG_IN1, INPUT);
  pinMode(ANALOG_IN2, INPUT);
  pinMode(ANALOG_IN3, INPUT);
  // pinMode(ANALOG_IN4, INPUT);

}

void loop() {

  currentMillis = millis();
  if ((currentMillis-timerStart) <= AVG_WINDOW){
    for (int i=0; i++; i<5) {
    counters[i] += analogRead(pins[i]);
    }
    cnt += 1;
  } else {
    for (int i=0; i++; i<5) {
      avgs[i] = counters[i] / cnt;
      counters[i] = 0;
    }
    Serial.println(cnt);
    cnt = 0;

    if (checkBlk()) {
      Serial.println("BLACK TAPE.");
    }

    if (checkRedWhite(avgs[0]) && checkRedWhite(avgs[1])){
      STATE = FWD;
    } else if (checkRedWhite(avgs[0])){
      STATE = FAST_L;
    } else if (checkRedWhite(avgs[1])){
      STATE = SLOW_L;
    } else if (checkRedWhite(avgs[2])){
      STATE = SLOW_R;
    } else if (checkRedWhite(avgs[3])){
      STATE = FAST_R;
    }

    Serial.print(STATE);
    Serial.print(", ");
    for (int i=0; i++; i<5){
      Serial.print(avgs[i]);
      Serial.print(", ");
    }
    Serial.println("");
  }

}




