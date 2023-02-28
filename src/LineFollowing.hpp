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
        bool checkSensor(int i_sensor, Colors_t color);
        bool checkAnySensor(Colors_t);

        int n_sensors;
        int avg_window;
        uint8_t pins[4];

    private:

        void calculateAverages();
        void updateAverages();
        void updateValues();

        int newest_idx; // index of newest value

        unsigned long int vals[4][20];
        float avgs[4];
        unsigned long int prev_vals[4];

        int wht_tape;
        int red_tape;
        int blk_tape;

        float inv_avg_window;

};

#endif
