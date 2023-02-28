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
    for (int i = 0; i < avg_window; i++){
        freq_vals[i] = 0;
    }

    newest_idx = avg_window;

    // upper and lower bounds for accepting a detected signal
    high_freq_range[0] = 3000; high_freq_range[1] = 3600;
    low_freq_range[0] = 750; low_freq_range[1] = 1050;
}

void BeaconSensing::Update(){
    updateValues();
    updateAverage();
}

bool BeaconSensing::checkForFrequency(Freqs_t high_or_low, Freq_modes_t freq_type) {

    int lb; 
    int ub;

    switch (high_or_low) {
        case HIGH_FREQ:
            lb = high_freq_range[0];
            ub = high_freq_range[1];
        case LOW_FREQ:
            lb = low_freq_range[0];
            ub = low_freq_range[1];
    }

    switch (freq_type){
        case AVERAGE:
            return ((getAvgFreq() >= lb) && (getAvgFreq() <= ub));
        case INSTANT:
            return ((getInstantFreq() >= lb) && (getInstantFreq() <= ub));
    }
    return false;
}

void BeaconSensing::updateEstimateFreq(float interrupt_freq){
    estimate_freq = interrupt_freq; // [hz] frequency of estimatation
    estimate_intv = 1./estimate_freq; // [s] interval between estimates

    // just to be safe, reset the average frequency and all values
    avg_freq = 0;
    for (int i = 0; i < avg_window; i++){
        freq_vals[i] = 0;
    }
    counter = 0;
}

float BeaconSensing::getAvgFreq(){
    return avg_freq;
}

int BeaconSensing::getInstantFreq(){
    return freq_vals[newest_idx];
}

void BeaconSensing::incrementCounter() {
    counter++;
}

void BeaconSensing::updateValues() {
    newest_idx++;

    if (newest_idx >= avg_window){
        newest_idx = 0;
    }
    prev_freq = freq_vals[newest_idx];

    freq_vals[newest_idx] = counter / estimate_intv;
    counter = 0;
}

void BeaconSensing::updateAverage() {
  // calculate average by updating only using the oldest/newest values
  avg_freq = avg_freq + inv_avg_window*(freq_vals[newest_idx] - prev_freq);
}


