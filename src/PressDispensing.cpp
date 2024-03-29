#include <PressDispensing.hpp>

//PressDispenser class constructor
PressDispensing::PressDispensing(int pd_pin) {
    pin = pd_pin;
    pinMode(pin, OUTPUT);
}

void PressDispensing::start(float dur) {
    digitalWrite(pin, HIGH);
    startTime = millis();
    running = true;
    duration = dur;
}

bool PressDispensing::isRunning() {
    return running;
}

void PressDispensing::monitorShutdown(float currentTime) {
    if ((currentTime - startTime) >= (duration * 1000)) {
        digitalWrite(pin, LOW);
        running = false;
    }
}