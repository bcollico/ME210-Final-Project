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
      meas_vals[i][j] = 1000; // initialize to white tape values
    }
  }

  red_tape = 500;
  blk_tape = 200;

  meas_bias[0] = 54;
  meas_bias[1] = -45;
  meas_bias[2] = -101;
  meas_bias[3] = -61;

}

void LineFollowing::Update(){
  // Serial.println("Update Function.");
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
    prev_vals[i] = meas_vals[i][newest_idx]; // store values to be removed
    meas_vals[i][newest_idx] = analogRead(pins[i]) - meas_bias[i]; // write in new values and subtract bias
    // Serial.println(vals[i][newest_idx]);
  }
}

void LineFollowing::calculateAverages(){
  // calculate averages by looping over full array
  for (int i = 0; i < n_sensors; i++) {
    avgs[i] = 0;
    for (int j = 0; j < avg_window; j++) {
      avgs[i] += meas_vals[i][j];
    }
    avgs[i] = (avgs[i] / avg_window);
  }
}

void LineFollowing::updateAverages() {
  // calculate averages by updating only using the oldest/newest values
  for (int i = 0; i < n_sensors; i++) {
    avgs[i] += inv_avg_window*(meas_vals[i][newest_idx] - prev_vals[i]);
  }

}

bool LineFollowing::checkSensor(Sensors_t i, Colors_t color){
  // check if the specified sensor is over the specified color
  if (i > n_sensors-1) {
    Serial.print("Index number "); Serial.print(i);
    Serial.print(" is outside of the index range 0-"); Serial.print(n_sensors-1);
    return false;
  }

  // Serial.println("logical output");
  // Serial.println((meas_vals[i][newest_idx] <= red_tape) && (meas_vals[i][newest_idx] >= blk_tape) == true);

  // Serial.println("Color");
  // Serial.println(color);


  switch (color) {
    case RED:
      if (((meas_vals[i][newest_idx] <= red_tape) && (meas_vals[i][newest_idx] >= blk_tape)) == true) {
        // Serial.println("RED.");
        return true;
        }
      break;
    case WHITE: 
      if ((meas_vals[i][newest_idx] > red_tape) == true) {
        // Serial.println("WHITE");
        return true;
      }
      break;
    case BLACK:
      if ((meas_vals[i][newest_idx] < blk_tape) == true) {
        // Serial.println("BLACK.");
        return true;
        }
      break;
  }


  // Serial.println("END.");
  return false;
  
}

bool LineFollowing::checkAnySensor(Colors_t color=BLACK){
  // check if any sensors are over the specified color
  for (int i = 0; i < n_sensors; i++) {
    switch (color) {
      case RED:
        if ((meas_vals[i][newest_idx] <= red_tape) && (meas_vals[i][newest_idx] >= blk_tape)) {return true;};
        break;
      case WHITE: 
        if (meas_vals[i][newest_idx] > red_tape) {return true;};
        break;
      case BLACK:
        if (meas_vals[i][newest_idx] < blk_tape) {return true;};
        break;
    }
  }
  return false;
}

void LineFollowing::calibrate_sensors(){

  int current_value;

  Serial.println("PLACE OVER RED TAPE");
  delay(5000);
  Serial.println("-------------START-------------");
  for (int i = 0; i < n_samples; i++) {
    for (int j = 0; j < n_sensors; j++) {
      current_value = analogRead(pins[j]);
      if (current_value > meas_bias[j] ) {
        meas_bias[j] = current_value;
      }
    }
    delay(100);
  }
  Serial.println("-------------STOP-------------");

  Serial.println("Red Tape Biases");
  for (int j = 0; j < n_sensors; j++){
    meas_bias[j] -= (red_tape+10);
    Serial.print(meas_bias[j]);
    Serial.print(", ");
  }
  Serial.println("");
}
