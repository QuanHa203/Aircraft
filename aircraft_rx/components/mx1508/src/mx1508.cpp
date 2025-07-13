#include "mx1508.h"

namespace aircraft_lib
{
    MX1508::MX1508(uint8_t motor_a_in1, uint8_t motor_a_in2, uint8_t motor_b_in1, uint8_t motor_b_in2)
        : motor_a_in1_(motor_a_in1), motor_a_in2_(motor_a_in2), motor_b_in1_(motor_b_in1), motor_b_in2_(motor_b_in2)
    {
        gpio_set_function(motor_a_in1_, GPIO_FUNC_PWM);
        gpio_set_function(motor_a_in2_, GPIO_FUNC_PWM);
        gpio_set_function(motor_b_in1_, GPIO_FUNC_PWM);
        gpio_set_function(motor_b_in2_, GPIO_FUNC_PWM);

        slice_motor_a_in1_ = pwm_gpio_to_slice_num(motor_a_in1_);
        slice_motor_a_in2_ = pwm_gpio_to_slice_num(motor_a_in2_);
        slice_motor_b_in1_ = pwm_gpio_to_slice_num(motor_b_in1_);
        slice_motor_b_in2_ = pwm_gpio_to_slice_num(motor_b_in2_);

        channel_motor_a_in1_ = pwm_gpio_to_channel(motor_a_in1_);
        channel_motor_a_in2_ = pwm_gpio_to_channel(motor_a_in2_);
        channel_motor_b_in1_ = pwm_gpio_to_channel(motor_b_in1_);
        channel_motor_b_in2_ = pwm_gpio_to_channel(motor_b_in2_);

        pwm_set_clkdiv(slice_motor_a_in1_, 4.0f);
        pwm_set_wrap(slice_motor_a_in1_, wrap_);
        pwm_set_enabled(slice_motor_a_in1_, true);

        pwm_set_clkdiv(slice_motor_a_in2_, 4.0f);
        pwm_set_wrap(slice_motor_a_in2_, wrap_);
        pwm_set_enabled(slice_motor_a_in2_, true);

        pwm_set_clkdiv(slice_motor_b_in1_, 4.0f);
        pwm_set_wrap(slice_motor_b_in1_, wrap_);
        pwm_set_enabled(slice_motor_b_in1_, true);

        pwm_set_clkdiv(slice_motor_b_in2_, 4.0f);
        pwm_set_wrap(slice_motor_b_in2_, wrap_);
        pwm_set_enabled(slice_motor_b_in2_, true);
    }

    void MX1508::set_pwm_duty(uint8_t slice, uint8_t channel, uint8_t duty_cycle)
    {
        uint level = (uint)((duty_cycle * wrap_) / 255);
        pwm_set_chan_level(slice, channel, level);
    }

    void MX1508::motor_A_forward(uint8_t duty_cycle)
    {
        if (!is_motor_a_forward_)
        {
            gpio_set_function(motor_a_in1_, GPIO_FUNC_PWM);
            gpio_set_function(motor_a_in2_, GPIO_FUNC_SIO);
            is_motor_a_forward_ = true;
        }

        set_pwm_duty(slice_motor_a_in1_, channel_motor_a_in1_, duty_cycle);
        gpio_put(motor_a_in2_, 0);
    }

    void MX1508::motor_A_backward(uint8_t duty_cycle)
    {
        if (!is_motor_a_backward_)
        {
            gpio_set_function(motor_a_in1_, GPIO_FUNC_SIO);
            gpio_set_function(motor_a_in2_, GPIO_FUNC_PWM);
            is_motor_a_backward_ = true;
        }
        
        gpio_put(motor_a_in1_, 0);
        set_pwm_duty(slice_motor_a_in2_, channel_motor_a_in2_, duty_cycle);
    }

    void MX1508::motor_A_stop()
    {
        gpio_set_function(motor_a_in1_, GPIO_FUNC_SIO);
        gpio_set_function(motor_a_in2_, GPIO_FUNC_SIO);
        gpio_put(motor_a_in1_, 0);
        gpio_put(motor_a_in2_, 0);

        is_motor_a_forward_ = false;
        is_motor_a_backward_ = false;
    }

    void MX1508::motor_B_forward(uint8_t duty_cycle)
    {
        if (!is_motor_b_forward_)
        {
            gpio_set_function(motor_b_in1_, GPIO_FUNC_SIO);
            gpio_set_function(motor_b_in2_, GPIO_FUNC_PWM);
            is_motor_b_forward_ = true;
        }

        gpio_put(motor_b_in1_, 0);
        set_pwm_duty(slice_motor_b_in2_, channel_motor_b_in2_, duty_cycle);
    }

    void MX1508::motor_B_backward(uint8_t duty_cycle)
    {
        if (!is_motor_b_backward_)
        {
            gpio_set_function(motor_b_in1_, GPIO_FUNC_PWM);
            gpio_set_function(motor_b_in2_, GPIO_FUNC_SIO);
            is_motor_b_backward_ = true;
        }

        set_pwm_duty(slice_motor_b_in1_, channel_motor_b_in1_, duty_cycle);
        gpio_put(motor_b_in2_, 0);
    }

    void MX1508::motor_B_stop()
    {
        gpio_set_function(motor_b_in1_, GPIO_FUNC_SIO);
        gpio_set_function(motor_b_in2_, GPIO_FUNC_SIO);
        gpio_put(motor_b_in1_, 0);
        gpio_put(motor_b_in2_, 0);

        is_motor_b_forward_ = false;
        is_motor_b_backward_ = false;
    }

} // namespace aircraft_lib
