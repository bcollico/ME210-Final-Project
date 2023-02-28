#include <Arduino.h>

#define ANALOG_IN0 A0
#define ANALOG_IN1 A1
#define ANALOG_IN2 A2
#define ANALOG_IN3 A3
// #define ANALOG_IN4 A4

int tape_0;
int tape_1;
int tape_2;
int tape_3;
uint8_t tapePins[] = {tape_0, tape_1, tape_2, tape_3};
// int tape_4;

int wht_tape = 400;
int red_tape = 250;
int blk_tape = 100;

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
    if (tapePins[i] <= blk_tape) {
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
  delay(100);

  tape_0 = analogRead(ANALOG_IN0);
  tape_1 = analogRead(ANALOG_IN1);
  tape_2 = analogRead(ANALOG_IN2);
  tape_3 = analogRead(ANALOG_IN3);
  // tape_4 = analogRead(ANALOG_IN4);

  // conditions:
  // tape_0 triggers fast_l
  // tape_1 triggers slow_l
  // tape_1 + tape_2 triggers fwd
  // tape_2 triggers slow_r
  // tape_3 triggers fast_r

  if (checkBlk()) {
    Serial.println("BLACK TAPE.");
  }

  if (checkRedWhite(tape_0) && checkRedWhite(tape_1)){
    STATE = FWD;
  } else if (checkRedWhite(tape_0)){
    STATE = FAST_L;
  } else if (checkRedWhite(tape_1)){
    STATE = SLOW_L;
  } else if (checkRedWhite(tape_2)){
    STATE = SLOW_R;
  } else if (checkRedWhite(tape_3)){
    STATE = FAST_R;
  }



  // switch (STATE){
  //   case (FAST_L):
  //     if (tape_0 >= red_tape) & 

    
  //   case (SLOW_L):

    
  //   case (FAST_R):

    
  //   case (SLOW_R):

    
  //   case (FWD):

  // }


  Serial.print(STATE);
  Serial.print(", ");
  Serial.print(tape_0);
  Serial.print(", ");
  Serial.print(tape_1);
  Serial.print(", ");
  Serial.print(tape_2);
  Serial.print(", ");
  Serial.print(tape_3);
  // Serial.print(", ");
  // Serial.print(tape_4);
  Serial.println("");
}




