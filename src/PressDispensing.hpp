#ifndef PressDispensing_hpp
#define PressDispensing_hpp
#include "Arduino.h"

#define PD_DURATION  5 //seconds

class PressDispensing {
    public:
    PressDispensing(int pd_pin);

    int duration;
    int pin;

    void start(int duration);
    bool isRunning();
    void monitorShutdown(int currentTime);

    private:
    int startTime;
    bool running;
};

#endif