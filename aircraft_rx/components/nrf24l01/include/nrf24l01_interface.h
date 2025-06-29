#ifndef NRF24L01_INTERFACE_H
#define NRF24L01_INTERFACE_H

#include "stdint.h"
#include "stddef.h"

typedef struct
{
    uint8_t frame_id;
    uint8_t throttle;
    uint8_t yaw;
    uint8_t pitch;
    uint8_t roll;
    uint8_t flags;
    uint8_t checksum;
    uint8_t reserved;

} nrf24l01_payload_packet;

class INrf24l01
{
private:
    virtual uint8_t read_register(uint8_t reg) = 0;
    virtual void write_register(uint8_t reg, uint8_t data) = 0;

    virtual void read_register_multi(uint8_t reg, uint8_t *data, size_t len) = 0;
    virtual void write_register_multi(uint8_t reg, uint8_t *data, size_t len) = 0;

    virtual void read_payload(uint8_t *data, size_t len) = 0;
    virtual void write_payload(uint8_t *data, size_t len) = 0;

    virtual void flush_tx() = 0;
    virtual void flush_rx() = 0;

    virtual void set_ce_pin_high() = 0;
    virtual void set_ce_pin_low() = 0;

    virtual void set_csn_pin_high() = 0;
    virtual void set_csn_pin_low() = 0;

    virtual void activate_features() = 0;
    virtual void clear_interrupt_flags() = 0;

public:
    virtual void init() = 0;

    virtual void config_tx_mode(uint8_t *tx_addr, uint8_t payload_len, uint8_t channel) = 0;
    virtual void config_rx_mode(uint8_t *rx_addr, uint8_t payload_len, uint8_t channel) = 0;

    virtual bool transmit(uint8_t *data, size_t len, uint32_t timeout_ms) = 0;
    virtual bool receive(uint8_t *data, size_t len) = 0;    

    virtual void print_register_map() = 0;

    ~INrf24l01() = default;
};

#endif
