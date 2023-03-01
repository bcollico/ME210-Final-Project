#ifndef MotorControl_hpp
#define MotorControl_hpp

#include "Arduino.h"

typedef enum {
    IDLE, FWD_FAST, FWD_SLOW, BWD_FAST, BWD_SLOW
} MotorState_t;

typedef enum {
  MOTOR_LEFT, MOTOR_RIGHT
} Motor_t;

typedef enum {
  CCW, CW
} WheelDirection_t;

// defines for motor speeds
#define FAST  20
#define SLOW  20
#define OFF   0

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
      MotorState_t L_motorState;
      MotorState_t R_motorState;


      void idle();
      void forward();
      void backward();
      void slowRight();
      void slowLeft();
      void fastRight();
      void fastLeft();

    private:
      void motorRotate(Motor_t motor, WheelDirection_t direction, int16_t speed);
};

#endif
