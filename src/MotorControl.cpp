#include <MotorControl.hpp>

//MotorControl class constructor
MotorControl::MotorControl(int l_epin, int r_epin, int l_dpin, int r_dpin) {
    L_epin = l_epin; // left enable pin
    R_epin = r_epin; // right enable pin
    L_dpin = l_dpin; // left direction pin
    R_dpin = r_dpin; // right direction pin

    L_dir = CCW;
    R_dir = CCW;
    L_speed = OFF;
    R_speed = OFF;
    L_motorState = IDLE;
    R_motorState = IDLE;

    pinMode(l_epin,OUTPUT);
    pinMode(r_epin,OUTPUT);
    pinMode(l_dpin,OUTPUT);
    pinMode(r_dpin,OUTPUT);
}

void MotorControl::motorRotate(Motor_t motor, WheelDirection_t direction, int16_t speed) {
  if (speed < 0) speed = 0;
  if (speed > 100) speed = 100;
  digitalWrite(motor == MOTOR_LEFT ? L_epin : R_epin, HIGH);
  analogWrite(motor == MOTOR_LEFT ? L_dpin : R_dpin, map((direction == CCW ? -1 * speed : speed),-100,100,0,255));
  // analogWrite(motor == MOTOR_LEFT ? L_epin : R_epin, map(speed,0,100,0,255));
  // digitalWrite(motor == MOTOR_LEFT ? L_dpin : R_dpin, direction);
}

void MotorControl::idle() {
  motorRotate(MOTOR_LEFT, CCW, OFF);
  motorRotate(MOTOR_RIGHT, CCW, OFF);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = OFF;
  R_speed = OFF;
  L_motorState = IDLE;
  R_motorState = IDLE;
}

void MotorControl::forward() {
  motorRotate(MOTOR_LEFT, CCW, FAST);
  motorRotate(MOTOR_RIGHT, CCW, FAST);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FAST;
  R_speed = FAST;
  L_motorState = FWD_FAST;
  R_motorState = FWD_FAST;
}

void MotorControl::backward() {
  motorRotate(MOTOR_LEFT, CW, FAST);
  motorRotate(MOTOR_RIGHT, CW, FAST);
  L_dir = CW;
  R_dir = CW;
  L_speed = FAST;
  R_speed = FAST;
  L_motorState = BWD_FAST;
  R_motorState = BWD_FAST;
}

void MotorControl::slowRight() {
  motorRotate(MOTOR_LEFT, CCW, SLOW);
  motorRotate(MOTOR_RIGHT, CW, SLOW);
  L_dir = CCW;
  R_dir = CW;
  L_speed = SLOW;
  R_speed = SLOW;
  L_motorState = FWD_SLOW;
  R_motorState = BWD_SLOW;
}

void MotorControl::slowLeft() {
  motorRotate(MOTOR_LEFT, CW, SLOW);
  motorRotate(MOTOR_RIGHT, CCW, SLOW);
  L_dir = CW;
  R_dir = CCW;
  L_speed = SLOW;
  R_speed = SLOW;
  L_motorState = BWD_SLOW;
  R_motorState = FWD_SLOW;
}

void MotorControl::fastRight() {
  motorRotate(MOTOR_LEFT, CCW, FAST);
  motorRotate(MOTOR_RIGHT, CW, FAST);
  L_dir = CCW;
  R_dir = CW;
  L_speed = FAST;
  R_speed = FAST;
  L_motorState = FWD_FAST;
  R_motorState = BWD_FAST;
}

void MotorControl::fastLeft() {
  motorRotate(MOTOR_LEFT, CW, FAST);
  motorRotate(MOTOR_RIGHT, CCW, FAST);
  L_dir = CW;
  R_dir = CCW;
  L_speed = FAST;
  R_speed = FAST;
  L_motorState = BWD_FAST;
  R_motorState = FWD_FAST;
}