#ifndef BeaconSensing_hpp
#define BeaconSensing_hpp

typedef enum {
    HIGH_FREQ, LOW_FREQ
} Freqs_t;

void interruptHandler(BeaconSensing Beacon);

class BeaconSensing {
    public:
      BeaconSensing(int pin_in, float interrupt_freq) ;
      void Update();
      void incrementCounter();

      int pin;
      float estimate_freq; // [hz] frequency of estimatation
      float estimate_intv; // [s] interval between estimates
    private:
      void updateAverage();
      void updateValues();

      unsigned int counter; // counter for number of falling edges
      int newest_idx; // index of newest value added to array

      int avg_window; // no. of values to average
      float inv_avg_window; // inverse to compute running average update

      unsigned int vals[10]; // frequency values to average
      unsigned int prev_val; // most recently overwritten value
      float avg_freq; // average frequency

      // range of frequency values to compare for high and low beacons
      unsigned int low_freq_range[2]; 
      unsigned int high_freq_range[2];
};

#endif
