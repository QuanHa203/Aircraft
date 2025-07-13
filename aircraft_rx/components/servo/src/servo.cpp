#include "servo.h"

namespace aircraft_lib
{
    Servo::Servo(uint8_t con_pin) : con_pin_(con_pin)
    {
        setup_servo_pwm();
    }

    void Servo::setup_servo_pwm()
    {
        gpio_set_function(con_pin_, GPIO_FUNC_PWM);
        slice_ = pwm_gpio_to_slice_num(con_pin_);
        channel_ = pwm_gpio_to_channel(con_pin_);

        pwm_set_wrap(slice_, 39062);   // wrap = 39062
        pwm_set_clkdiv(slice_, 64.0f); // clkdiv = 64.0f
        pwm_set_enabled(slice_, true);
    }

    void Servo::set_servo_angle(uint8_t angle)
    {
        uint32_t pulse_us = 500 + (angle * 2000) / 180;
        uint32_t level = (pulse_us * 39062) / 20000;
        pwm_set_chan_level(slice_, channel_, level);
    }
} // namespace aircraft_lib
