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

    motor_pos_ctrl_flag = false;
    motor_yaw_ctrl_flag = false;
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

void MotorControl::motorRotate(Motor_t motor, WheelDirection_t direction, float speed) {
  if (speed < 0.0) speed = 0.0;
  if (speed > 100.0) speed = 100.0;
  digitalWrite(motor == MOTOR_LEFT ? L_epin : R_epin, HIGH);
  analogWrite(motor == MOTOR_LEFT ? L_dpin : R_dpin, map((direction == CW ? -1 * speed : speed),-100,100,0,255));
  // analogWrite(motor == MOTOR_LEFT ? L_epin : R_epin, map(speed,0,100,0,255));
  // digitalWrite(motor == MOTOR_LEFT ? L_dpin : R_dpin, direction);
}


void MotorControl::setControlParams(float kp_yaw_, float kp_pos_, float f_ctrl_) {
  kp_yaw = kp_yaw_;
  kp_pos = kp_pos_;
  f_ctrl = f_ctrl_;
  T_ctrl = 1./f_ctrl;
}

void MotorControl::StartHeadingControl(float yaw_desired) {
  yaw_d = yaw_desired;
  ctrl_timer_start = millis();
  motor_yaw_ctrl_flag = true;
}

void MotorControl::StartPositionControl(float pos_desired) {
  pos_d = pos_desired;
  ctrl_timer_start = millis();
  motor_pos_ctrl_flag = true;
}

bool MotorControl::HeadingControlActive() {
    return motor_yaw_ctrl_flag;
}

bool MotorControl::PositionControlActive() {
    return motor_pos_ctrl_flag;
}

void MotorControl::ControlHeading(float yaw, int current_millis) {

  // control motor if we've not reached our desired change
  // and at the desired control frequency
  if ((abs(yaw - yaw_d) > 0.035) && motor_yaw_ctrl_flag) {
    if ((current_millis - ctrl_timer_start) > T_ctrl) {
      ctrl_timer_start = millis();

      motor_cmd = SaturateControl((yaw_d-yaw) * (180/PI) * kp_yaw);

      if (motor_cmd > 0) {
        motorRotate(MOTOR_RIGHT, CW, motor_cmd);
        motorRotate(MOTOR_LEFT, CCW, motor_cmd);
      } else {
        motorRotate(MOTOR_RIGHT,CCW, abs(motor_cmd));
        motorRotate(MOTOR_LEFT,  CW, abs(motor_cmd));
      }
    }
  } else {
    idle();
    // motor_yaw_ctrl_flag = false;
  }
}

void MotorControl::ControlPosition(float pos, int current_millis) {

  // control motor if we've not reached our desired change
  // and at the desired control frequency
  if ((abs(pos_d - pos) > 0.05) && motor_pos_ctrl_flag) {
    if ((current_millis - ctrl_timer_start) > T_ctrl) {
      ctrl_timer_start = millis();

      motor_cmd = SaturateControl((pos_d - pos) * kp_pos);

      if (motor_cmd > SLOW) {
        motor_cmd = SLOW;
      }

      if (motor_cmd > 0) {
        motorRotate(MOTOR_RIGHT, CW, motor_cmd);
        motorRotate(MOTOR_LEFT , CW, motor_cmd);
      } else {
        motorRotate(MOTOR_RIGHT,CCW, motor_cmd);
        motorRotate(MOTOR_LEFT, CCW, motor_cmd);
      }
      }
  } else {
    idle();
    // motor_pos_ctrl_flag = false;
  }
}

float MotorControl::SaturateControl(float input) {
  if (input > SLOW) {
    return SLOW;
  } else if (input < -SLOW) {
    return -SLOW;
  } else if ((input > 0) && (input < 15)) {
    return 15;
  } else if ((input < 0) && (input > -15)) {
    return -15;
  } else {
    return input;
  }
}
