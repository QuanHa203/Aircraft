#ifndef NRF24L01_INTERFACE_H
#define NRF24L01_INTERFACE_H

#include "stdint.h"
#include "stddef.h"

class INrf24l01
{
private:

    virtual void read_payload(uint8_t* data, size_t len) = 0;
    virtual void write_payload(uint8_t* data, size_t len) = 0;

    virtual void set_ce_pin_high() = 0;
    virtual void set_ce_pin_low() = 0;
    
    virtual void set_csn_pin_high() = 0;
    virtual void set_csn_pin_low() = 0;

    virtual void clear_interrupt_flags() = 0;
    
public:
    virtual void init() = 0;

    // virtual void setChannel(uint8_t channel) = 0;
    // virtual void setSpeed(uint8_t speed) = 0;
    // virtual void setAutoAcknowledge(bool isEnable) = 0;
    // virtual void setPayloadWidth(size_t length) = 0;

    virtual uint8_t  read_register(uint8_t reg) = 0;
    virtual void write_register(uint8_t reg, uint8_t data) = 0;

    virtual void read_register_multi(uint8_t reg, uint8_t* data, size_t len) = 0;
    virtual void write_register_multi(uint8_t reg, uint8_t* data, size_t len) = 0;


    virtual void config_tx_mode(uint8_t* tx_addr, uint8_t payload_len, uint8_t channel) = 0;
    virtual void config_rx_mode(uint8_t* rx_addr, uint8_t payload_len, uint8_t channel, uint8_t pipe) = 0;

    virtual bool transmit(uint8_t *data, size_t len, uint32_t timeout_ms) = 0;
    virtual bool receive(uint8_t *data, size_t len) = 0;

    virtual void flush_tx() = 0;
    virtual void flush_rx() = 0;

    virtual void activate_features() = 0;

    virtual void print_register_map() = 0;

    ~INrf24l01() = default;
};


#endif
