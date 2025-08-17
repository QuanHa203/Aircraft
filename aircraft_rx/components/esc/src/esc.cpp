#include "esc.h"

namespace aircraft_lib
{
    ESC::ESC(uint8_t pin) : pin_(pin)
    {
        gpio_set_function(pin_, GPIO_FUNC_PWM);
        slice_num_ = pwm_gpio_to_slice_num(pin_);
        channel_ = pwm_gpio_to_channel(pin_);

        // Default clock  125 MHz / 64 = 1.953125 MHz
        // Wrap = 39062 ~ 50HZ
        pwm_set_clkdiv(slice_num_, 64.f);
        pwm_set_wrap(slice_num_, 39062);
        pwm_set_enabled(slice_num_, true);
    }

    void ESC::set_speed(uint8_t percent)
    {
        int us = 10 * percent + 1000;
        uint16_t level = (us * 39062) / 20000;
        pwm_set_chan_level(slice_num_, channel_, level);
    }
}