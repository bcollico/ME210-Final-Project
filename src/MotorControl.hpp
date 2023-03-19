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
      void fastForward();
      void slowForward();
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
      void intoStudio();

      // General movement public function
      void moveBot(WheelDirection_t l_dir, WheelDirection_t r_dir, int l_speed, int r_speed);

    private:

      void motorRotate(Motor_t motor, WheelDirection_t direction, int speed);
      void motorRotate(Motor_t motor, WheelDirection_t direction, float speed);
};

#endif
