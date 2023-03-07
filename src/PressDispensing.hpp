#ifndef PressDispensing_hpp
#define PressDispensing_hpp
#include "Arduino.h"

#define PD_DURATION  0.3 //seconds

class PressDispensing {
    public:
    PressDispensing(int pd_pin);

    float duration;
    int pin;

    void start(float duration);
    bool isRunning();
    void monitorShutdown(float currentTime);

    private:
    int startTime;
    bool running;
};

#endif