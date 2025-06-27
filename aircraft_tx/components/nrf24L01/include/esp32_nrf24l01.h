#ifndef ESP32_NRF24L01_H
#define ESP32_NRF24L01_H

#include "nrf24l01_interface.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "freertos/FreeRTOS.h"

#include "esp_rom_sys.h"
#include "esp_log.h"

#include "string.h"

namespace aircraft_lib
{

    class Esp32Nrf24l01 : INrf24l01
    {
    private:
        // SPI command
        const uint8_t NRF24L01_CMD_R_REGISTER = 0x00;
        const uint8_t NRF24L01_CMD_W_REGISTER = 0x20;
        const uint8_t NRF24L01_CMD_R_RX_PAYLOAD = 0x61;
        const uint8_t NRF24L01_CMD_W_TX_PAYLOAD = 0xA0;
        const uint8_t NRF24L01_CMD_FLUSH_TX = 0xE1;
        const uint8_t NRF24L01_CMD_FLUSH_RX = 0xE2;
        const uint8_t NRF24L01_CMD_REUSE_TX_PL = 0xE3;
        const uint8_t NRF24L01_CMD_R_RX_PL_WID = 0x60;
        const uint8_t NRF24L01_CMD_W_ACK_PAYLOAD = 0x15;
        const uint8_t NRF24L01_CMD_NOP = 0xFF;

        const uint8_t NRF24L01_CONFIG = 0x00;
        const uint8_t NRF24L01_EN_AA = 0x01;
        const uint8_t NRF24L01_EN_RXADDR = 0x02;
        const uint8_t NRF24L01_SETUP_AW = 0x03;
        const uint8_t NRF24L01_SETUP_RETR = 0x04;
        const uint8_t NRF24L01_RF_CH = 0x05;
        const uint8_t NRF24L01_RF_SETUP = 0x06;
        const uint8_t NRF24L01_STATUS = 0x07;
        const uint8_t NRF24L01_OBSERVE_TX = 0x08;
        const uint8_t NRF24L01_RPD = 0x09;
        const uint8_t NRF24L01_RX_ADDR_P0 = 0x0A;
        const uint8_t NRF24L01_RX_ADDR_P1 = 0x0B;
        const uint8_t NRF24L01_RX_ADDR_P2 = 0x0C;
        const uint8_t NRF24L01_RX_ADDR_P3 = 0x0D;
        const uint8_t NRF24L01_RX_ADDR_P4 = 0x0E;
        const uint8_t NRF24L01_RX_ADDR_P5 = 0x0F;
        const uint8_t NRF24L01_TX_ADDR = 0x10;
        const uint8_t NRF24L01_RX_PW_P0 = 0x11;
        const uint8_t NRF24L01_RX_PW_P1 = 0x12;
        const uint8_t NRF24L01_RX_PW_P2 = 0x13;
        const uint8_t NRF24L01_RX_PW_P3 = 0x14;
        const uint8_t NRF24L01_RX_PW_P4 = 0x15;
        const uint8_t NRF24L01_RX_PW_P5 = 0x16;
        const uint8_t NRF24L01_FIFO_STATUS = 0x17;
        const uint8_t NRF24L01_DYNPD = 0x1C;
        const uint8_t NRF24L01_FEATURE = 0x1D;

        const uint8_t NRF24L01_STATUS_MAX_RT = (1 << 4);
        const uint8_t NRF24L01_STATUS_TX_DS = (1 << 5);
        const uint8_t NRF24L01_STATUS_RX_DR = (1 << 6);

        gpio_num_t ce_pin_, csn_pin_, irq_pin_, sck_pin_, mosi_pin_, miso_pin_;
        static spi_device_handle_t nrf24l01_spi_;

        uint8_t read_register(uint8_t reg) override;
        void write_register(uint8_t reg, uint8_t data) override;

        void read_register_multi(uint8_t reg, uint8_t *data, size_t len) override;
        void write_register_multi(uint8_t reg, uint8_t *data, size_t len) override;

        void read_payload(uint8_t *data, size_t len) override;
        void write_payload(uint8_t *data, size_t len) override;

        void set_ce_pin_high() override;
        void set_ce_pin_low() override;

        void set_csn_pin_high() override;
        void set_csn_pin_low() override;

        void flush_tx() override;
        void flush_rx() override;

        void clear_interrupt_flags() override;

    public:
        Esp32Nrf24l01(gpio_num_t ce_pin, gpio_num_t csn_pin, gpio_num_t irq_pin, gpio_num_t sck_pin, gpio_num_t mosi_pin, gpio_num_t miso_pin);

        void init() override;

        void config_tx_mode(uint8_t *tx_addr, uint8_t payload_len, uint8_t channel) override;
        void config_rx_mode(uint8_t *rx_addr, uint8_t payload_len, uint8_t channel, uint8_t pipe) override;

        bool transmit(uint8_t *data, size_t len, uint32_t timeout_ms) override;
        bool receive(uint8_t *data, size_t len) override;

        void activate_features() override;

        void print_register_map() override;
    };
}
#endif