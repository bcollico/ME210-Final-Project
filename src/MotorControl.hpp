#ifndef MotorControl_hpp
#define MotorControl_hpp

#include "Arduino.h"
#include "Parameters.hpp"

typedef enum {
  MOTOR_LEFT, MOTOR_RIGHT
} Motor_t;

typedef enum {
  CCW, CW
} WheelDirection_t;

// #define FAST  30
// #define SLOW  30
// #define OFF   0
// #define HARDFWD_DIFF  20 // %
// #define SOFTFWD_DIFF  10  // %

class MotorControl {
    public:
      MotorControl(int l_epin, int r_epin, int l_dpin, int r_dpin);

      int L_epin;
      int R_epin;
      int L_dpin;
      int R_dpin;
      int L_dir;
      int R_dir;
      int L_speed;
      int R_speed;


      void idle();
      void forward();
      void backward();
      void slowRight();
      void slowLeft();
      void fastRight();
      void fastLeft();
      void hardFwdRight();
      void hardFwdRight(int diff);
      void hardFwdLeft();
      void hardFwdLeft(int diff);
      void softFwdRight();
      void softFwdRight(int diff);
      void softFwdLeft();
      void softFwdLeft(int diff);

      // General movement public function
      void moveBot(WheelDirection_t l_dir, WheelDirection_t r_dir, int l_speed, int r_speed);

      // motor control
      void setControlParams(float kp_yaw_, float kp_pos_, float f_ctrl_);
      void StartHeadingControl(float yaw_desired);
      void StartPositionControl(float pos_desired);
      bool HeadingControlActive();
      bool PositionControlActive();
      void ControlHeading(float yaw, int current_millis);
      void ControlPosition(float yaw, int current_millis); 

    private:

    // 
      void motorRotate(Motor_t motor, WheelDirection_t direction, int speed);
      void motorRotate(Motor_t motor, WheelDirection_t direction, float speed);

      float SaturateControl(float in);

      bool motor_pos_ctrl_flag;
      bool motor_yaw_ctrl_flag;
      float motor_cmd; // control command
      float yaw_d; // desired yaw
      float pos_d; // desired yaw
      float kp_yaw; // proportional control gain
      float kp_pos; // proportional control gain
      float f_ctrl; // control frequency
      float T_ctrl; // control period
      int ctrl_timer_start; // control timer
};

#endif
