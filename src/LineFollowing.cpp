#include <Arduino.h>
#include <LineFollowing.hpp>

// line following class constructor
LineFollowing::LineFollowing() {

  n_sensors = 4; // no. of line sensors
  avg_window = 20; // no. of values to average
  newest_idx = avg_window; // index of newest value

  inv_avg_window = 1./avg_window; // for updated running average

  const uint8_t allpins[] = {A0, A1, A2, A3, A4, A5};

  for (int i = 0; i < n_sensors; i++){
    pins[i] = allpins[i];
    prev_vals[i] = 1000; // initialize for updating averages
    avgs[i] = 1000; // initialize for updating averages
    pinMode(pins[i], INPUT);
    for (int j = 0; j < avg_window; j++){
      vals[i][j] = 1000; // initialize to white tape values
    }
  }

  wht_tape = 400;
  red_tape = 250;
  blk_tape = 100;

}

void LineFollowing::Update(){
  // add a new sensor value to each sensor array
  updateValues();

  // update averages
  updateAverages();
}

void LineFollowing::updateValues(){
  newest_idx++;

  // reset the index if end of array is reached
  if (newest_idx >= avg_window){
    newest_idx = 0;
  }

  for (int i = 0; i < n_sensors; i++){
    prev_vals[i] = vals[i][newest_idx]; // store values to be removed
    vals[i][newest_idx] = analogRead(pins[i]); // write in new values
  }
}

void LineFollowing::calculateAverages(){
  // calculate averages by looping over full array
  for (int i = 0; i < n_sensors; i++) {
    avgs[i] = 0;
    for (int j = 0; j < avg_window; j++) {
      avgs[i] += vals[i][j];
    }
    avgs[i] = avgs[i] / avg_window;
  }
}

void LineFollowing::updateAverages() {
  // calculate averages by updating only using the oldest/newest values
  for (int i = 0; i < n_sensors; i++) {
    avgs[i] = avgs[i] + inv_avg_window*(vals[i][newest_idx] - prev_vals[i]);
  }

}

bool LineFollowing::checkSensor(int i_sensor, Colors_t color=RED){
  // check if the specified sensor is over the specified color
  if (i_sensor > n_sensors-1) {
    Serial.print("Index number "); Serial.print(i_sensor);
    Serial.print(" is outside of the index range 0-"); Serial.print(n_sensors-1);
    return false;
  }

  switch (color) {
    case RED:
      return avgs[i_sensor] >= red_tape && avgs[i_sensor] < wht_tape;
    case WHITE: 
      return avgs[i_sensor] >= wht_tape;
    case BLACK:
      return avgs[i_sensor] <= blk_tape;

    return false;
  }
}

bool LineFollowing::checkAnySensor(Colors_t color=BLACK){
  // check if any sensors are over the specified color
  for (int i = 0; i < n_sensors; i++) {
    switch (color) {
      case RED:
        if (avgs[i] >= red_tape && avgs[i] < wht_tape) {return true;};
      case WHITE: 
        if (avgs[i] >= wht_tape) {return true;};
      case BLACK:
        if (avgs[i] <= blk_tape) {return true;};
    }
  }
  return false;
}




