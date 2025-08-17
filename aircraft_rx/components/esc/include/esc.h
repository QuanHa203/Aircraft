#ifndef ESC_H
#define ESC_H

#include "pico/stdio.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

namespace aircraft_lib
{
    class ESC
    {
    private:
        uint8_t pin_;
        uint8_t slice_num_;
        uint8_t channel_;
    public:
        ESC(uint8_t pin);
        void set_speed(uint8_t percent);
    };
}

#endif