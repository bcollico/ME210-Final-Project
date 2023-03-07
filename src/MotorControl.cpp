#include <MotorControl.hpp>

//MotorControl class constructor
MotorControl::MotorControl(int l_epin, int r_epin, int l_dpin, int r_dpin) {
    L_epin = l_epin; // left enable pin
    R_epin = r_epin; // right enable pin
    L_dpin = l_dpin; // left direction pin
    R_dpin = r_dpin; // right direction pin

    L_dir = CW;
    R_dir = CW;
    L_speed = OFF;
    R_speed = OFF;

    pinMode(l_epin,OUTPUT);
    pinMode(r_epin,OUTPUT);
    pinMode(l_dpin,OUTPUT);
    pinMode(r_dpin,OUTPUT);
}

void MotorControl::idle() {
  motorRotate(MOTOR_LEFT, CCW, OFF);
  motorRotate(MOTOR_RIGHT, CCW, OFF);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = OFF;
  R_speed = OFF;
}

void MotorControl::forward() {
  motorRotate(MOTOR_LEFT, CCW, FAST);
  motorRotate(MOTOR_RIGHT, CCW, FAST);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FAST;
  R_speed = FAST;
}

void MotorControl::backward() {
  motorRotate(MOTOR_LEFT, CW, SLOW);
  motorRotate(MOTOR_RIGHT, CW, SLOW);
  L_dir = CW;
  R_dir = CW;
  L_speed = SLOW;
  R_speed = SLOW;
}

void MotorControl::slowRight() {
  motorRotate(MOTOR_LEFT, CCW, SLOW);
  motorRotate(MOTOR_RIGHT, CW, SLOW);
  L_dir = CCW;
  R_dir = CW;
  L_speed = SLOW;
  R_speed = SLOW;
}

void MotorControl::slowLeft() {
  motorRotate(MOTOR_LEFT, CW, SLOW);
  motorRotate(MOTOR_RIGHT, CCW, SLOW);
  L_dir = CW;
  R_dir = CCW;
  L_speed = SLOW;
  R_speed = SLOW;
}

void MotorControl::fastRight() {
  motorRotate(MOTOR_LEFT, CCW, FAST);
  motorRotate(MOTOR_RIGHT, CW, FAST);
  L_dir = CCW;
  R_dir = CW;
  L_speed = FAST;
  R_speed = FAST;
}

void MotorControl::fastLeft() {
  motorRotate(MOTOR_LEFT, CW, FAST);
  motorRotate(MOTOR_RIGHT, CCW, FAST);
  L_dir = CW;
  R_dir = CCW;
  L_speed = FAST;
  R_speed = FAST;
}

void MotorControl::hardFwdRight() {
  motorRotate(MOTOR_LEFT, CCW, FAST);
  motorRotate(MOTOR_RIGHT, CCW, FAST - HARDFWD_DIFF);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FAST;
  R_speed = FAST - HARDFWD_DIFF;
}

void MotorControl::hardFwdRight(int diff) {
  motorRotate(MOTOR_LEFT, CCW, FAST);
  motorRotate(MOTOR_RIGHT, CCW, FAST - diff);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FAST;
  R_speed = FAST - diff;
}

void MotorControl::hardFwdLeft() {
  motorRotate(MOTOR_LEFT, CCW, FAST - HARDFWD_DIFF);
  motorRotate(MOTOR_RIGHT, CCW, FAST);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FAST - HARDFWD_DIFF;
  R_speed = FAST;
}

void MotorControl::hardFwdLeft(int diff) {
  motorRotate(MOTOR_LEFT, CCW, FAST - diff);
  motorRotate(MOTOR_RIGHT, CCW, FAST);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FAST - diff;
  R_speed = FAST;
}

void MotorControl::softFwdRight() {
  motorRotate(MOTOR_LEFT, CCW, FAST);
  motorRotate(MOTOR_RIGHT, CCW, FAST - SOFTFWD_DIFF);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FAST;
  R_speed = FAST - SOFTFWD_DIFF;
}

void MotorControl::softFwdRight(int diff) {
  motorRotate(MOTOR_LEFT, CCW, FAST);
  motorRotate(MOTOR_RIGHT, CCW, FAST - diff);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FAST;
  R_speed = FAST - diff;
}

void MotorControl::softFwdLeft() {
  motorRotate(MOTOR_LEFT, CCW, FAST - SOFTFWD_DIFF);
  motorRotate(MOTOR_RIGHT, CCW, FAST);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FAST - SOFTFWD_DIFF;
  R_speed = FAST;
}

void MotorControl::softFwdLeft(int diff) {
  motorRotate(MOTOR_LEFT, CCW, FAST - diff);
  motorRotate(MOTOR_RIGHT, CCW, FAST);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FAST - diff;
  R_speed = FAST;
}

void MotorControl::moveBot(WheelDirection_t l_dir, WheelDirection_t r_dir, int l_speed, int r_speed) {
  L_dir = l_dir;
  R_dir = r_dir;
  L_speed = l_speed;
  R_speed = r_speed;
  motorRotate(MOTOR_LEFT, l_dir, l_speed);
  motorRotate(MOTOR_RIGHT, r_dir, r_speed);
}

void MotorControl::motorRotate(Motor_t motor, WheelDirection_t direction, int speed) {
  if (speed < 0) speed = 0;
  if (speed > 100) speed = 100;
  digitalWrite(motor == MOTOR_LEFT ? L_epin : R_epin, HIGH);
  analogWrite(motor == MOTOR_LEFT ? L_dpin : R_dpin, map((direction == CW ? -1 * speed : speed),-100,100,0,255));
  // analogWrite(motor == MOTOR_LEFT ? L_epin : R_epin, map(speed,0,100,0,255));
  // digitalWrite(motor == MOTOR_LEFT ? L_dpin : R_dpin, direction);
}
