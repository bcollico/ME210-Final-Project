#include <Arduino.h>
#include <BeaconSensing.hpp>

void interruptHandler(BeaconSensing Beacon){
    Beacon.Update();
}

// BeaconSensing class constructor
BeaconSensing::BeaconSensing(int pin_in, float interrupt_freq) {
    pin = pin_in;
    counter = 0;

    estimate_freq = interrupt_freq; // [hz] frequency of estimatation
    estimate_intv = 1./estimate_freq; // [s] interval between estimates

    pinMode(pin, INPUT);

    avg_window = 10;
    inv_avg_window = 1./avg_window;

    avg_freq = 0;
    for (unsigned int i = 0; i < avg_window; i++){
        vals[i] = 0;
    }

    newest_idx = avg_window;
}

float BeaconSensing::getAverage(){
    return avg_freq;
}


void BeaconSensing::Update(){
    updateValues();
    updateAverage();
}

void BeaconSensing::incrementCounter() {
    counter++;
}

void BeaconSensing::updateValues() {
    newest_idx++;

    if (newest_idx >= avg_window){
        newest_idx = 0;
    }
    prev_val = vals[newest_idx];

    vals[newest_idx] = digitalRead(pin);
}

void BeaconSensing::updateAverage() {
  // calculate average by updating only using the oldest/newest values
  avg_freq = avg_freq + inv_avg_window*(vals[newest_idx] - prev_val);
}


