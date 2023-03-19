#ifndef Parameters_hpp
#define Parameters_hpp

#include "Arduino.h"

// pin definitions
#define L_DPIN        9
#define L_EPIN        7
#define R_DPIN        10
#define R_EPIN        12
#define IR_PIN_IN     2
#define CP_PIN        6
#define PD_PIN        8
#define FREQ_SWITCH_PIN 3
#define RESET_PIN     4

// Crowd Pleaser Parameters
#define CP_DURATION  2 //seconds

// Press Dispenser Parameters
#define PD_DURATION  0.75 //seconds

// Motor Control Parameters
#define OFF   0
#define FAST 60
#define SLOW 30
#define TURN_FAST 35
#define TURN_SLOW 30
#define FWD_LF 36
#define HARDFWD_DIFF 24
#define SOFTFWD_DIFF 16
#define INTOSTUDIO_DIFF 8

#endif