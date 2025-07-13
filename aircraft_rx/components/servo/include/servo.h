#ifndef SERVO_H
#define SERVO_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

namespace aircraft_lib
{
    class Servo
    {
    private:
        uint8_t con_pin_;
        uint8_t slice_;
        uint8_t channel_;

    public:
        Servo(uint8_t con_pin);
        void setup_servo_pwm();
        void set_servo_angle(uint8_t angle);
    };

} // namespace aircraft_lib

#endif