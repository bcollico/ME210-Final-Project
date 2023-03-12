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
#define FAST  30
#define SLOW  20
#define OFF   0
#define HARDFWD_DIFF  20 // %
#define SOFTFWD_DIFF  10  // %

#endif