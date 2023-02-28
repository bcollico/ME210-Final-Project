#ifndef BeaconSensing_hpp
#define BeaconSensing_hpp

typedef enum {
    HIGH_FREQ, LOW_FREQ
} Freqs_t;

typedef enum {
    AVERAGE, INSTANT
} Freq_modes_t;

class BeaconSensing {
    public:
      BeaconSensing(int pin_in, float interrupt_freq) ;
      void Update();
      void incrementCounter();
      void updateEstimateFreq(float freq);
      bool checkForFrequency(Freqs_t high_or_low, Freq_modes_t freq_type = AVERAGE);

      int pin;
      float estimate_freq; // [hz] frequency of estimatation
      float estimate_intv; // [s] interval between estimates

      int freq_vals[10]; // frequency values to average
      float avg_freq; // average frequency
      int newest_idx; // index of newest value added to array

    private:
      void updateAverage();
      void updateValues();

      int getInstantFreq();
      float getAvgFreq();

      int counter; // counter for number of falling edges

      int avg_window; // no. of values to average
      float inv_avg_window; // inverse to compute running average update

      int prev_freq; // most recently overwritten value

      // range of frequency values to compare for high and low beacons
      int low_freq_range[2]; 
      int high_freq_range[2];
};

#endif
