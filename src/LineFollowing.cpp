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

<<<<<<< Updated upstream
  lower_red[0] = 229; //150; //266; //229;
  lower_red[1] = 142; //100; //162; //149;
  lower_red[2] = 119; //100; // 118; //130;
  lower_red[3] = 125; //100; //117; //136;

  upper_red[0] = 600; //650; //726; //543; //550;//776; //679;
  upper_red[1] = 550; //600; //640; //377; //300;//483; //460;
  upper_red[2] = 440; //480; //540; //317; //300;//369; //394;
  upper_red[3] = 440; //480; //539; //351; //300;//371; //415;
=======
  lower_red[0] = 126;
  lower_red[1] = 74;
  lower_red[2] = 45;
  lower_red[3] = 32;

  upper_red[0] = 370;
  upper_red[1] = 239;
  upper_red[2] = 214;
  upper_red[3] = 266;
>>>>>>> Stashed changes

  blk_high[0] = -1000;
  blk_high[1] = -1000;
  blk_high[2] = -1000;
  blk_high[3] = -1000;

  white_low[0] = 2000;
  white_low[1] = 2000;
  white_low[2] = 2000;
  white_low[3] = 2000;

  red_margin = 0.2; // 20% margin between black and white before reaching red
}

void LineFollowing::Update(){
  // Serial.println("Update Function.");
  // add a new sensor value to each sensor array
  updateValues();

  // update averages
  // updateAverages();
}

void LineFollowing::updateValues(){
  newest_idx++;

  // reset the index if end of array is reached
  if (newest_idx >= avg_window){
    newest_idx = 0;
  }

  for (int i = 0; i < n_sensors; i++){
    prev_vals[i] = meas_vals[i][newest_idx]; // store values to be removed
    meas_vals[i][newest_idx] = analogRead(pins[i]); // write in new values
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
      if (((meas_vals[i][newest_idx] <= upper_red[i]) && (meas_vals[i][newest_idx] >= lower_red[i])) == true) {
        // Serial.println("RED.");
        return true;
        }
      break;
    case WHITE: 
      if ((meas_vals[i][newest_idx] > upper_red[i]) == true) {
        // Serial.println("WHITE");
        return true;
      }
      break;
    case BLACK:
      if ((meas_vals[i][newest_idx] < lower_red[i]) == true) {
        // Serial.println("BLACK.");
        return true;
        break;
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
        if ((meas_vals[i][newest_idx] <= upper_red[i]) && (meas_vals[i][newest_idx] >= lower_red[i])) {return true;};
        break;
      case WHITE: 
        if (meas_vals[i][newest_idx] > upper_red[i]) {return true;};
        break;
      case BLACK:
        if (meas_vals[i][newest_idx] < lower_red[i]) {return true;};
        break;
    }
  }
  return false;
}

void LineFollowing::calibrate_sensors(){

  int current_value;

  Serial.println("PLACE OVER BLACK TAPE");
  delay(5000);
  Serial.println("-------------START-------------");
  for (int i = 0; i < n_samples; i++) {
    for (int j = 0; j < n_sensors; j++) {
      current_value = analogRead(pins[j]);
      // Serial.println(analogRead(pins[j]));
      if (current_value > blk_high[j] ) {
        blk_high[j] = current_value;
      }
    }
    delay(100);
  }
  Serial.println("-------------STOP-------------");
  delay(1000);
  Serial.println("PLACE OVER WHITE AREA");
  delay(5000);
  Serial.println("-------------START-------------");
  for (int i = 0; i < n_samples; i++) {
    for (int j = 0; j < n_sensors; j++) {
      current_value = analogRead(pins[j]);
      // Serial.println(analogRead(pins[j]));
      if (current_value < white_low[j] ) {
        white_low[j] = current_value;
      }
    }
    delay(100);
  }
  Serial.println("-------------STOP-------------");

  lower_red[0] = blk_high[0] + red_margin * (white_low[0] - blk_high[0]);
  lower_red[1] = blk_high[1] + red_margin * (white_low[1] - blk_high[1]);
  lower_red[2] = blk_high[2] + red_margin * (white_low[2] - blk_high[2]);
  lower_red[3] = blk_high[3] + red_margin * (white_low[3] - blk_high[3]);

  upper_red[0] = white_low[0] - 0.3 * (white_low[0] - blk_high[0]);
  upper_red[1] = white_low[1] - 0.3 * (white_low[1] - blk_high[1]);
  upper_red[2] = white_low[2] - 0.3 * (white_low[2] - blk_high[2]);
  upper_red[3] = white_low[3] - 0.3 * (white_low[3] - blk_high[3]);

  Serial.println("Red Tape Thresholds");
  for (int j = 0; j < n_sensors; j++){
    Serial.print(lower_red[j]);
    Serial.print(", ");
    Serial.println(upper_red[j]);
    // Serial.print(blk_high[0]);
    // Serial.print(", ");
    // Serial.println(white_low[j]);
    // Serial.println("");
  }
  Serial.println("");
}
