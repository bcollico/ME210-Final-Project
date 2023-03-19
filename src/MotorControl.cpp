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

void MotorControl::fastForward() {
  motorRotate(MOTOR_LEFT, CCW, FAST);
  motorRotate(MOTOR_RIGHT, CCW, FAST);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FAST;
  R_speed = FAST;
}

void MotorControl::slowForward() {
  motorRotate(MOTOR_LEFT, CCW, SLOW);
  motorRotate(MOTOR_RIGHT, CCW, SLOW);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = SLOW;
  R_speed = SLOW;
}

void MotorControl::backward() {
  motorRotate(MOTOR_LEFT, CW, FAST);
  motorRotate(MOTOR_RIGHT, CW, FAST);
  L_dir = CW;
  R_dir = CW;
  L_speed = FAST;
  R_speed = FAST;
}

void MotorControl::slowRight() {
  motorRotate(MOTOR_LEFT, CCW, TURN_SLOW);
  motorRotate(MOTOR_RIGHT, CW, TURN_SLOW);
  L_dir = CCW;
  R_dir = CW;
  L_speed = TURN_SLOW;
  R_speed = TURN_SLOW;
}

void MotorControl::slowLeft() {
  motorRotate(MOTOR_LEFT, CW, TURN_SLOW);
  motorRotate(MOTOR_RIGHT, CCW, TURN_SLOW);
  L_dir = CW;
  R_dir = CCW;
  L_speed = TURN_SLOW;
  R_speed = TURN_SLOW;
}

void MotorControl::fastRight() {
  motorRotate(MOTOR_LEFT, CCW, TURN_FAST);
  motorRotate(MOTOR_RIGHT, CW, TURN_FAST);
  L_dir = CCW;
  R_dir = CW;
  L_speed = TURN_FAST;
  R_speed = TURN_FAST;
}

void MotorControl::fastLeft() {
  motorRotate(MOTOR_LEFT, CW, TURN_FAST);
  motorRotate(MOTOR_RIGHT, CCW, TURN_FAST);
  L_dir = CW;
  R_dir = CCW;
  L_speed = TURN_FAST;
  R_speed = TURN_FAST;
}

void MotorControl::hardFwdRight() {
  motorRotate(MOTOR_LEFT, CCW, FWD_LF);
  motorRotate(MOTOR_RIGHT, CCW, FWD_LF - HARDFWD_DIFF);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FWD_LF;
  R_speed = FWD_LF - HARDFWD_DIFF;
}

void MotorControl::hardFwdRight(int diff) {
  motorRotate(MOTOR_LEFT, CCW, FWD_LF);
  motorRotate(MOTOR_RIGHT, CCW, FWD_LF - diff);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FWD_LF;
  R_speed = FWD_LF - diff;
}

void MotorControl::hardFwdLeft() {
  motorRotate(MOTOR_LEFT, CCW, FWD_LF - HARDFWD_DIFF);
  motorRotate(MOTOR_RIGHT, CCW, FWD_LF);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FWD_LF - HARDFWD_DIFF;
  R_speed = FWD_LF;
}

void MotorControl::hardFwdLeft(int diff) {
  motorRotate(MOTOR_LEFT, CCW, FWD_LF - diff);
  motorRotate(MOTOR_RIGHT, CCW, FWD_LF);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FWD_LF - diff;
  R_speed = FWD_LF;
}

void MotorControl::softFwdRight() {
  motorRotate(MOTOR_LEFT, CCW, FWD_LF);
  motorRotate(MOTOR_RIGHT, CCW, FWD_LF - SOFTFWD_DIFF);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FWD_LF;
  R_speed = FWD_LF - SOFTFWD_DIFF;
}

void MotorControl::softFwdRight(int diff) {
  motorRotate(MOTOR_LEFT, CCW, FWD_LF);
  motorRotate(MOTOR_RIGHT, CCW, FWD_LF - diff);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FWD_LF;
  R_speed = FWD_LF - diff;
}

void MotorControl::softFwdLeft() {
  motorRotate(MOTOR_LEFT, CCW, FWD_LF - SOFTFWD_DIFF);
  motorRotate(MOTOR_RIGHT, CCW, FWD_LF);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FWD_LF - SOFTFWD_DIFF;
  R_speed = FWD_LF;
}

void MotorControl::softFwdLeft(int diff) {
  motorRotate(MOTOR_LEFT, CCW, FWD_LF - diff);
  motorRotate(MOTOR_RIGHT, CCW, FWD_LF);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FWD_LF - diff;
  R_speed = FWD_LF;
}

void MotorControl::intoStudio() {
  motorRotate(MOTOR_LEFT, CCW, FWD_LF - INTOSTUDIO_DIFF);
  motorRotate(MOTOR_RIGHT, CCW, FWD_LF);
  L_dir = CCW;
  R_dir = CCW;
  L_speed = FWD_LF - INTOSTUDIO_DIFF;
  R_speed = FWD_LF;
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
}
<<<<<<< Updated upstream
=======

void MotorControl::motorRotate(Motor_t motor, WheelDirection_t direction, float speed) {
  if (speed < 0.0) speed = 0.0;
  if (speed > 100.0) speed = 100.0;
  digitalWrite(motor == MOTOR_LEFT ? L_epin : R_epin, HIGH);
  analogWrite(motor == MOTOR_LEFT ? L_dpin : R_dpin, map((direction == CW ? -1 * speed : speed),-100,100,0,255));
}
>>>>>>> Stashed changes
