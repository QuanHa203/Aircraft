extern "C"
{
#include "FreeRTOS.h"
#include "task.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
}

#include "esc.h"
#include "nrf24l01_interface.h"
#include "rp2040_nrf24l01.h"
#include "mx1508.h"
#include "servo.h"

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

#define ESC_PIN 14

const uint8_t payload_len = 8;
const uint8_t channel = 110;

uint8_t rx_address[5] = {'P', 'L', 'A', 'N', 'E'};

nrf24l01_payload_packet packet[2];
volatile uint8_t packet_write_index = 0;
volatile uint8_t packet_read_index = 0;

aircraft_lib::RP2040Nrf24l01 rp2040_rf_rx(spi1, CE_PIN, CSN_PIN, IRQ_PIN, SCK_PIN, MOSI_PIN, MISO_PIN);
INrf24l01 *rf_rx = &rp2040_rf_rx;

aircraft_lib::ESC esc(ESC_PIN);
aircraft_lib::Servo servo_left(SERVO_LEFT_PIN);
aircraft_lib::Servo servo_right(SERVO_RIGHT_PIN);

int clamp(int value, int min, int max);
uint abs(int num);
void control_motor();
void control_servo();

void handle_control_aircraft_task(void *arg);
void handle_get_packet_from_aircraft_tx_task(void *arg);

void vApplicationMallocFailedHook(void);
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);

extern "C" int main()
{
    stdio_init_all();
    sleep_ms(1000);

    rf_rx->init();
    rf_rx->config_rx_mode(rx_address, payload_len, channel);
    printf("Init success RF RX!\n");

    servo_left.set_servo_angle(90);
    servo_right.set_servo_angle(90);

    esc.set_speed(0);
    sleep_ms(2000);

    xTaskCreate(handle_get_packet_from_aircraft_tx_task, "task_get_packet", 1024, NULL, 4, NULL);
    xTaskCreate(handle_control_aircraft_task, "task_control", 1024, NULL, 4, NULL);
    vTaskStartScheduler();
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
    while (1)
    {
        if (packet_read_index == packet_write_index)
            continue;

        packet_read_index = packet_write_index;

        control_motor();
        control_servo();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void handle_get_packet_from_aircraft_tx_task(void *arg)
{
    uint8_t rx_payload[payload_len];
    while (1)
    {
        if (rf_rx->receive(rx_payload, payload_len))
        {
            uint8_t index = packet_write_index ^ 1;
            packet[index].frame_id = rx_payload[0];
            packet[index].throttle = rx_payload[1];
            packet[index].yaw = rx_payload[2];
            packet[index].pitch = rx_payload[3];
            packet[index].roll = rx_payload[4];
            packet[index].flags = rx_payload[5];
            packet[index].checksum = rx_payload[6];
            packet[index].reserved = rx_payload[7];
            packet_write_index = index;

            printf("Receive from tx: \"%u %u %u %u %u %u %u %u\"\n", rx_payload[0], rx_payload[1], rx_payload[2], rx_payload[3], rx_payload[4], rx_payload[5], rx_payload[6], rx_payload[7]);
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

void vApplicationMallocFailedHook(void)
{
    printf("Malloc failed!\n");
    while (1)
        ;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("Stack overflow in task: %s\n", pcTaskName);
    while (1)
        ;
}

void control_motor()
{
    static uint8_t last_throttle = -1;
    uint8_t throttle = packet[packet_read_index].throttle;
    uint8_t speed_percent;
    if (last_throttle == throttle)
        return;

    last_throttle = throttle;
    speed_percent = 100 * throttle / 255;

    esc.set_speed(speed_percent);
}

void control_servo()
{
    static uint8_t last_roll = -1;
    static uint8_t last_pitch = -1;

    uint8_t roll = packet[packet_read_index].roll;
    uint8_t pitch = packet[packet_read_index].pitch;

    if (last_roll == roll && last_pitch == pitch)
        return;

    last_roll = roll;
    last_pitch = pitch;

    u_int8_t servo_left_angle;  // 0° - 180°
    u_int8_t servo_right_angle; // 0° - 180°

    int16_t roll_offset = (int16_t)roll - 127;
    int16_t pitch_offset = (int16_t)pitch - 127;

    int16_t left_mix = 127 + pitch_offset + roll_offset;
    int16_t right_mix = 127 + pitch_offset - roll_offset;

    left_mix = clamp(left_mix, 0, 255);
    right_mix = clamp(right_mix, 0, 255);

    servo_left_angle = (uint16_t)(left_mix * 180 + 127) / 255;
    servo_right_angle = (uint16_t)(right_mix * 180 + 127) / 255;

    servo_left.set_servo_angle(servo_left_angle);
    servo_right.set_servo_angle(abs(servo_right_angle - 180));
}