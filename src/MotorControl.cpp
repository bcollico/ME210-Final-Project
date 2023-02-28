#include <MotorControl.hpp>

//MotorControl class constructor
MotorControl::MotorControl(int l_epin, int r_epin, int l_dpin, int r_dpin) {
    L_epin = l_epin; // left enable pin
    R_epin = r_epin; // right enable pin
    L_dpin = l_dpin; // left direction pin
    R_dpin = r_dpin; // right direction pin

    L_dir = CCW;
    R_dir = CCW;
    L_speed = 0;
    R_speed = 0;
    L_motorState = IDLE;
    R_motorState = IDLE;
}

void MotorControl::motorRotate(Motor_t motor, int direction, int16_t speed) {
  if (speed < 0) speed = 0;
  if (speed > 100) speed = 100;
  analogWrite(motor == MOTOR_LEFT ? L_epin : R_epin, map(speed,0,100,0,255));
  digitalWrite(motor == MOTOR_LEFT ? L_dpin : R_dpin, direction);
}

void MotorControl::idle() {
  motorRotate(MOTOR_LEFT, CCW, 0);
  motorRotate(MOTOR_RIGHT, CCW, 0);
  L_motorState = IDLE;
  R_motorState = IDLE;
}

void MotorControl::forward() {
  motorRotate(MOTOR_LEFT, CCW, 80);
  motorRotate(MOTOR_RIGHT, CCW, 80);
  L_motorState = FWD_FAST;
  R_motorState = FWD_FAST;
}

void MotorControl::backward() {
  motorRotate(MOTOR_LEFT, CW, 80);
  motorRotate(MOTOR_RIGHT, CW, 80);
  L_motorState = BWD_FAST;
  R_motorState = BWD_FAST;
}

void MotorControl::slowRight() {
  motorRotate(MOTOR_LEFT, CCW, 50);
  motorRotate(MOTOR_RIGHT, CW, 50);
  L_motorState = FWD_SLOW;
  R_motorState = BWD_SLOW;
}

void MotorControl::slowLeft() {
  motorRotate(MOTOR_LEFT, CW, 50);
  motorRotate(MOTOR_RIGHT, CCW, 50);
  L_motorState = BWD_SLOW;
  R_motorState = FWD_SLOW;
}

void MotorControl::fastRight() {
  motorRotate(MOTOR_LEFT, CCW, 80);
  motorRotate(MOTOR_RIGHT, CW, 80);
  L_motorState = FWD_FAST;
  R_motorState = BWD_FAST;
}

void MotorControl::fastLeft() {
  motorRotate(MOTOR_LEFT, CW, 80);
  motorRotate(MOTOR_RIGHT, CCW, 80);
  L_motorState = BWD_FAST;
  R_motorState = FWD_FAST;
}