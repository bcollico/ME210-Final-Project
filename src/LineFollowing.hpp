#ifndef LineFollowing_hpp
#define LineFollowing_hpp

#include "Arduino.h"

typedef enum {
  RED, WHITE, BLACK
} Colors_t;

typedef enum {
    LEFT, LEFT_MID, RIGHT_MID, RIGHT
} Sensors_t;

class LineFollowing {
    public:
        LineFollowing();

        void Update();
        bool checkSensor(Sensors_t i_sensor, Colors_t color);
        bool checkAnySensor(Colors_t);
        void calibrate_sensors();

        int n_sensors;
        int avg_window;
        uint8_t pins[4];

        float avgs[4];
        int meas_vals[4][20];
        int meas_bias[4];
        int newest_idx; // index of newest value

    private:

        void calculateAverages();
        void updateAverages();
        void updateValues();

        int prev_vals[4];

        float inv_avg_window;

        int blk_tape;
        int red_tape;

        int n_samples = 100;

};

#endif
