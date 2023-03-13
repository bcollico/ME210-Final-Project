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
        int newest_idx; // index of newest value
        int upper_red[4];
        int lower_red[4];
        int blk_high[4];
        int white_low[4];
        int red_low[4];
        int red_high[4];

        float red_margin;

    private:

        void calculateAverages();
        void updateAverages();
        void updateValues();

        int prev_vals[4];

        float inv_avg_window;

        int n_samples = 100;

};

#endif
