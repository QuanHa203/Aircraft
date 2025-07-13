#ifndef MX1508_H
#define MX1508_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

namespace aircraft_lib
{
    class MX1508
    {
    private:
        uint8_t motor_a_in1_, motor_a_in2_, motor_b_in1_, motor_b_in2_;
        uint8_t slice_motor_a_in1_, slice_motor_a_in2_, slice_motor_b_in1_, slice_motor_b_in2_;
        uint8_t channel_motor_a_in1_, channel_motor_a_in2_, channel_motor_b_in1_, channel_motor_b_in2_;
        uint16_t wrap_ = 62500;
        bool is_motor_a_forward_ = false;
        bool is_motor_a_backward_ = false;
        bool is_motor_b_forward_ = false;
        bool is_motor_b_backward_ = false;

        void set_pwm_duty(uint8_t slice, uint8_t channel, uint8_t duty_cycle);
        
    public:
        MX1508(uint8_t motor_a_in1, uint8_t motor_a_in2, uint8_t motor_b_in1, uint8_t motor_b_in2);

        void motor_A_forward(uint8_t duty_cycle);
        void motor_A_backward(uint8_t duty_cycle);
        void motor_A_stop();

        void motor_B_forward(uint8_t duty_cycle);
        void motor_B_backward(uint8_t duty_cycle);
        void motor_B_stop();
    };
}

#endif