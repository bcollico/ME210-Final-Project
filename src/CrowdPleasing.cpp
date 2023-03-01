#include <CrowdPleasing.hpp>

//CrowdPleasing class constructor
CrowdPleasing::CrowdPleasing(int cp_pin) {
    pin = cp_pin;
    pinMode(pin, OUTPUT);
}

// Give the people what they want
void CrowdPleasing::start(int dur) {
    digitalWrite(pin, HIGH);
    startTime = millis();
    running = true;
    duration = dur;
}

bool CrowdPleasing::isRunning() {
    return running;
}

void CrowdPleasing::monitorShutdown(int currentTime) {
    if ((currentTime - startTime) >= (duration * 1000)) {
        digitalWrite(pin, LOW);
        running = false;
    }
}