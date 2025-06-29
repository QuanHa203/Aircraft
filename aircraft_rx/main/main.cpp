#include "nrf24l01_interface.h"
#include "rp2040_nrf24l01.h"

extern "C"
{
#include "pico/stdlib.h"
}

#define CE_PIN 8
#define CSN_PIN 9
#define IRQ_PIN 13
#define SCK_PIN 10
#define MOSI_PIN 11
#define MISO_PIN 12

uint8_t rx_address[5] = {'P', 'L', 'A', 'N', 'E'};
uint8_t payload_len = 32;
uint8_t channel = 110;
uint8_t rx_payload[32];

aircraft_lib::RP2040Nrf24l01 rp2040_rf_rx(spi1, CE_PIN, CSN_PIN, IRQ_PIN, SCK_PIN, MOSI_PIN, MISO_PIN);
INrf24l01 *rf_rx = &rp2040_rf_rx;


extern "C" int main()
{
    stdio_uart_init();
    sleep_ms(2000);

    rf_rx->init();
    rf_rx->config_rx_mode(rx_address, payload_len, channel);

    uint8_t buffer[32];
    while (1)
    {
        if (rf_rx->receive(buffer, sizeof(buffer)))
        {
            printf("[RX] Nhận được: \"%.*s\"\n", payload_len, buffer);
        }
        else
        {
            // nrf.print_register_map();
        }

        sleep_ms(100);
    }
}