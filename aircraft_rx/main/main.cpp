#include "nrf24l01_interface.h"
#include "rp2040_nrf24l01.h"
#include "mx1508.h"
#include "servo.h"

extern "C"
{
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
}

#define SERVO_LEFT_PIN 28
#define SERVO_RIGHT_PIN 29

#define MOTOR_A_IN1_PIN 4 // IN1
#define MOTOR_A_IN2_PIN 5 // IN2
#define MOTOR_B_IN1_PIN 6 // IN3
#define MOTOR_B_IN2_PIN 7 // IN4

#define CE_PIN 8
#define CSN_PIN 9
#define SCK_PIN 10
#define MOSI_PIN 11
#define MISO_PIN 12
#define IRQ_PIN 13

const uint8_t payload_len = 8;
const uint8_t channel = 110;

bool is_motor_spinning = false;
uint32_t last_motor_spinning_time_us = 0;

uint8_t rx_address[5] = {'P', 'L', 'A', 'N', 'E'};

nrf24l01_payload_packet packet;
volatile bool is_packet_ready = false;

aircraft_lib::RP2040Nrf24l01 rp2040_rf_rx(spi1, CE_PIN, CSN_PIN, IRQ_PIN, SCK_PIN, MOSI_PIN, MISO_PIN);
INrf24l01 *rf_rx = &rp2040_rf_rx;

aircraft_lib::MX1508 mx1508(MOTOR_A_IN1_PIN, MOTOR_A_IN2_PIN, MOTOR_B_IN1_PIN, MOTOR_B_IN2_PIN);
aircraft_lib::Servo servo_left(SERVO_LEFT_PIN);
aircraft_lib::Servo servo_right(SERVO_RIGHT_PIN);

int clamp(int value, int min, int max);
uint abs(int num);
void control_motor();
void control_servo();

void handle_control_aircraft_task(void *arg);
void handle_get_packet_from_aircraft_tx_task(void *arg);
void vApplicationMallocFailedHook(void);

extern "C" int main()
{
    stdio_uart_init();
    sleep_ms(2000);

    rf_rx->init();
    rf_rx->config_rx_mode(rx_address, payload_len, channel);

    servo_left.set_servo_angle(90);
    servo_right.set_servo_angle(90);

    mx1508.motor_A_stop();
    mx1508.motor_B_stop();

    uint8_t rx_payload[payload_len];
    while (1)
    {
        // if (!is_packet_ready)
        //     continue;

        if (rf_rx->receive(rx_payload, payload_len))
        {
            packet.frame_id = rx_payload[0];
            packet.throttle = rx_payload[1];
            packet.yaw = rx_payload[2];
            packet.pitch = rx_payload[3];
            packet.roll = rx_payload[4];
            packet.flags = rx_payload[5];
            packet.checksum = rx_payload[6];
            packet.reserved = rx_payload[7];

            is_packet_ready = true;
            // printf("Receive from tx: \"%u %u %u %u %u %u %u %u\"\n", rx_payload[0], rx_payload[1], rx_payload[2], rx_payload[3], rx_payload[4], rx_payload[5], rx_payload[6], rx_payload[7]);
        }

        control_motor();
        control_servo();
        is_packet_ready = false;
    }
    // Run Core 1
    // multicore_launch_core1(handle_get_packet_from_aircraft_tx_task);

    // xTaskCreate(handle_get_packet_from_aircraft_tx_task, "b", 4096, NULL, 4, NULL);
    // xTaskCreate(handle_control_aircraft_task, "a", 4096, NULL, 4, NULL);
}

int clamp(int value, int min, int max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

uint abs(int num)
{
    if (num >= 0)
        return num;
    return num * (-1);
}

void handle_control_aircraft_task(void *arg)
{
    uint8_t rx_payload[payload_len];

    while (1)
    {
        sleep_ms(20);

        // if (!is_packet_ready)
        //     continue;

        if (rf_rx->receive(rx_payload, payload_len))
        {
            packet.frame_id = rx_payload[0];
            packet.throttle = rx_payload[1];
            packet.yaw = rx_payload[2];
            packet.pitch = rx_payload[3];
            packet.roll = rx_payload[4];
            packet.flags = rx_payload[5];
            packet.checksum = rx_payload[6];
            packet.reserved = rx_payload[7];

            is_packet_ready = true;
            // printf("Receive from tx: \"%u %u %u %u %u %u %u %u\"\n", rx_payload[0], rx_payload[1], rx_payload[2], rx_payload[3], rx_payload[4], rx_payload[5], rx_payload[6], rx_payload[7]);
        }

        control_motor();
        control_servo();
        is_packet_ready = false;
    }
}

void handle_get_packet_from_aircraft_tx_task(void *arg)
{
    uint8_t rx_payload[payload_len];

    while (1)
    {
        if (rf_rx->receive(rx_payload, payload_len))
        {
            packet.frame_id = rx_payload[0];
            packet.throttle = rx_payload[1];
            packet.yaw = rx_payload[2];
            packet.pitch = rx_payload[3];
            packet.roll = rx_payload[4];
            packet.flags = rx_payload[5];
            packet.checksum = rx_payload[6];
            packet.reserved = rx_payload[7];

            is_packet_ready = true;
            // printf("Receive from tx: \"%u %u %u %u %u %u %u %u\"\n", rx_payload[0], rx_payload[1], rx_payload[2], rx_payload[3], rx_payload[4], rx_payload[5], rx_payload[6], rx_payload[7]);
        }
        sleep_ms(20);
    }
}

void vApplicationMallocFailedHook(void)
{
    while (1)
    {
        // debug hoặc reset tại đây
    }
}

void control_motor()
{
    uint8_t throttle = packet.throttle;

    // Stop motor
    if (throttle <= 20)
    {
        mx1508.motor_A_stop();
        mx1508.motor_B_stop();
        is_motor_spinning = false;
        return;
    }
    
    // Boot success
    if (time_us_32() - last_motor_spinning_time_us > 200.000)
    {
        mx1508.motor_A_forward(throttle);
        mx1508.motor_B_forward(throttle);
    }

    // Boot (200ms)
    if (!is_motor_spinning)
    {
        mx1508.motor_A_forward(255);
        mx1508.motor_B_forward(255);
        last_motor_spinning_time_us = time_us_32();
        is_motor_spinning = true;
        return;
    }
}

void control_servo()
{
    u_int8_t servo_left_angle;  // 0° - 180°
    u_int8_t servo_right_angle; // 0° - 180°

    int16_t roll_offset = (int16_t)packet.roll - 127;
    int16_t pitch_offset = (int16_t)packet.pitch - 127;

    int16_t left_mix = 127 + pitch_offset + roll_offset;
    int16_t right_mix = 127 + pitch_offset - roll_offset;

    left_mix = clamp(left_mix, 0, 255);
    right_mix = clamp(right_mix, 0, 255);

    servo_left_angle = (uint16_t)(left_mix * 180 + 127) / 255;
    servo_right_angle = (uint16_t)(right_mix * 180 + 127) / 255;
        
    servo_left.set_servo_angle(servo_left_angle);
    servo_right.set_servo_angle(abs(servo_right_angle - 180));
}