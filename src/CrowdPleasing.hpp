#ifndef CrowdPleasing_hpp
#define CrowdPleasing_hpp

#include "Arduino.h"

#define CP_DURATION  2 //seconds

class CrowdPleasing {
    public:
    CrowdPleasing(int cp_pin);

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
