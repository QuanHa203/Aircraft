#include "rp2040_nrf24l01.h"

namespace aircraft_lib
{
    // Contructor
    RP2040Nrf24l01::RP2040Nrf24l01(spi_inst_t* spi_port, uint8_t ce_pin, uint8_t csn_pin, uint8_t irq_pin, uint8_t sck_pin, uint8_t mosi_pin, uint8_t miso_pin)
        : spi_port_(spi_port), ce_pin_(ce_pin), csn_pin_(csn_pin), irq_pin_(irq_pin), sck_pin_(sck_pin), mosi_pin_(mosi_pin), miso_pin_(miso_pin) {}

    // Private method
    uint8_t RP2040Nrf24l01::read_register(uint8_t reg)
    {
        uint8_t cmd = NRF24L01_CMD_R_REGISTER | (reg & 0x1F);
        uint8_t tx_buf[2] = {cmd, NRF24L01_CMD_NOP};
        uint8_t rx_buf[2];

        set_csn_pin_low();
        spi_write_read_blocking(spi_port_, tx_buf, rx_buf, 2);
        set_csn_pin_high();
        return rx_buf[1];
    }

    void RP2040Nrf24l01::write_register(uint8_t reg, uint8_t data)
    {
        uint8_t cmd = NRF24L01_CMD_W_REGISTER | (reg & 0x1F);
        uint8_t tx_buf[2] = {cmd, data};
     
        set_csn_pin_low();
        spi_write_blocking(spi_port_, tx_buf, 2);
        set_csn_pin_high();
    }

    void RP2040Nrf24l01::read_register_multi(uint8_t reg, uint8_t *data, size_t len)
    {
        if (len > 31)
            return;

        uint8_t cmd = NRF24L01_CMD_R_REGISTER | (reg & 0x1F);
        uint8_t tx_buf[1 + len];
        uint8_t rx_buf[1 + len];
        memset(tx_buf, NRF24L01_CMD_NOP, sizeof(tx_buf));
        tx_buf[0] = cmd;

        set_csn_pin_low();
        spi_write_read_blocking(spi_port_, tx_buf, rx_buf, len + 1);
        set_csn_pin_high();
        memcpy(data, &rx_buf[1], len);  // Bỏ byte đầu (status)
    }
    void RP2040Nrf24l01::write_register_multi(uint8_t reg, uint8_t *data, size_t len)
    {
        uint8_t cmd = NRF24L01_CMD_W_REGISTER | (reg & 0x1F);
        uint8_t tx_buf[1 + len];
        tx_buf[0] = cmd;
        memcpy(&tx_buf[1], data, len);

        set_csn_pin_low();
        spi_write_blocking(spi_port_, tx_buf, len + 1);
        set_csn_pin_high();
    }

    void RP2040Nrf24l01::read_payload(uint8_t *data, size_t len)
    {
        if (len > 32)
            len = 32;

        uint8_t tx_buf[1 + len];
        uint8_t rx_buf[1 + len];

        tx_buf[0] = NRF24L01_CMD_R_RX_PAYLOAD;
        memset(&tx_buf[1], NRF24L01_CMD_NOP, len);

        set_csn_pin_low();
        spi_write_read_blocking(spi_port_, tx_buf, rx_buf, len + 1);
        set_csn_pin_high();

        memcpy(data, &rx_buf[1], len);  // // Bỏ byte đầu (status)
    }

    void RP2040Nrf24l01::write_payload(uint8_t *data, size_t len)
    {
        if (len > 32)
            len = 32;

        uint8_t tx_buf[1 + len];
        tx_buf[0] = NRF24L01_CMD_W_TX_PAYLOAD;
        memcpy(&tx_buf[1], data, len);

        set_csn_pin_low();
        spi_write_blocking(spi_port_, tx_buf, len + 1);
        set_csn_pin_high();
    }

    void RP2040Nrf24l01::flush_tx()
    {
        uint8_t cmd = NRF24L01_CMD_FLUSH_TX;

        set_csn_pin_low();
        spi_write_blocking(spi_port_, &cmd, 1);
        set_csn_pin_high();
    }

    void RP2040Nrf24l01::flush_rx()
    {
        uint8_t cmd = NRF24L01_CMD_FLUSH_RX;
        
        set_csn_pin_low();
        spi_write_blocking(spi_port_, &cmd, 1);
        set_csn_pin_high();
    }

    void RP2040Nrf24l01::set_ce_pin_high()
    {
        gpio_put(ce_pin_, 1);
    }

    void RP2040Nrf24l01::set_ce_pin_low()
    {
        gpio_put(ce_pin_, 0);
    }

    void RP2040Nrf24l01::set_csn_pin_high()
    {
        gpio_put(csn_pin_, 1);
    }

    void RP2040Nrf24l01::set_csn_pin_low()
    {
        gpio_put(csn_pin_, 0);
    }

    void RP2040Nrf24l01::activate_features()
    {
        uint8_t activate_cmd[2] = {0x50, 0x73};
        
        set_csn_pin_low();
        spi_write_blocking(spi_port_, activate_cmd, 2);
        set_csn_pin_high();
        
    }

    void RP2040Nrf24l01::clear_interrupt_flags()
    {
        write_register(NRF24L01_STATUS,
                       NRF24L01_STATUS_RX_DR | NRF24L01_STATUS_TX_DS | NRF24L01_STATUS_MAX_RT);
    }


    // Public method
    void RP2040Nrf24l01::init()
    {
        // Configure CE and CSN pin
        gpio_init(csn_pin_);
        gpio_init(ce_pin_);
        
        gpio_set_dir(csn_pin_, GPIO_OUT);
        gpio_set_dir(ce_pin_, GPIO_OUT);

        gpio_put(csn_pin_, 1); // Disconnect default SPI
        gpio_put(ce_pin_, 0);  // Disable TX/RX mode

        spi_init(spi_port_, 2 * 1000 * 1000); // // 2 MHz
        gpio_set_function(sck_pin_, GPIO_FUNC_SPI);
        gpio_set_function(miso_pin_, GPIO_FUNC_SPI);
        gpio_set_function(mosi_pin_, GPIO_FUNC_SPI);

        activate_features();
    }    

    void RP2040Nrf24l01::config_tx_mode(uint8_t *tx_addr, uint8_t payload_len, uint8_t channel)
    {
        set_ce_pin_low();

        write_register(NRF24L01_CONFIG, 0x7E);

        write_register(NRF24L01_EN_AA, 0x00);

        write_register(NRF24L01_EN_RXADDR, 0x01);

        write_register(NRF24L01_SETUP_AW, 0x03);

        write_register(NRF24L01_SETUP_RETR, 0x00);

        write_register(NRF24L01_RF_CH, channel);

        write_register(NRF24L01_RF_SETUP, 0x26);

        write_register_multi(NRF24L01_RX_ADDR_P0, tx_addr, 5);

        write_register_multi(NRF24L01_TX_ADDR, tx_addr, 5);

        write_register(NRF24L01_RX_PW_P0, payload_len);

        write_register(NRF24L01_DYNPD, 0x00);

        write_register(NRF24L01_FEATURE, 0x01);

        sleep_ms(2);
        clear_interrupt_flags();
        flush_tx();
    }

    void RP2040Nrf24l01::config_rx_mode(uint8_t *rx_addr, uint8_t payload_len, uint8_t channel)
    {
        set_ce_pin_low();

        write_register(NRF24L01_CONFIG, 0x7F);

        write_register(NRF24L01_EN_AA, 0x00);

        write_register(NRF24L01_EN_RXADDR, 0x01); // Pipe 0

        write_register(NRF24L01_SETUP_AW, 0x03);

        write_register(NRF24L01_SETUP_RETR, 0x00);

        write_register(NRF24L01_RF_CH, channel);

        write_register(NRF24L01_RF_SETUP, 0x26);

        write_register_multi(NRF24L01_RX_ADDR_P0, rx_addr, 5);

        write_register_multi(NRF24L01_TX_ADDR, rx_addr, 5);

        write_register(NRF24L01_RX_PW_P0, payload_len);

        write_register(NRF24L01_DYNPD, 0x00);

        write_register(NRF24L01_FEATURE, 0x01);

        sleep_ms(2);
        clear_interrupt_flags();
        flush_rx();

        set_ce_pin_high();
    }

    bool RP2040Nrf24l01::transmit(uint8_t *data, size_t len, uint32_t timeout_ms)
    {
        clear_interrupt_flags();

        // 1. Flush FIFO TX trước khi gửi mới
        flush_tx();

        // 2. Ghi payload vào FIFO TX
        write_payload(data, len);

        // 3. Pulse CE >=10µs để gửi
        set_ce_pin_high();
        sleep_us(15); // Gửi 15µs là đủ
        set_ce_pin_low();

        // 4. Poll STATUS để kiểm tra kết quả
        uint32_t start = to_ms_since_boot(get_absolute_time());
        while ((to_ms_since_boot(get_absolute_time()) - start) < timeout_ms)
        {
            uint8_t status = read_register(NRF24L01_STATUS);
            if (status & NRF24L01_STATUS_TX_DS)
            {
                // Gửi thành công
                write_register(NRF24L01_STATUS, NRF24L01_STATUS_TX_DS); // Clear flag
                return true;
            }
            else if (status & NRF24L01_STATUS_MAX_RT)
            {
                // Gửi thất bại sau max retry
                write_register(NRF24L01_STATUS, NRF24L01_STATUS_MAX_RT); // Clear flag
                flush_tx();                                              // Dọn FIFO
                return false;
            }

            sleep_ms(1);
        }

        // Hết timeout
        return false;
    }

    bool RP2040Nrf24l01::receive(uint8_t *data, size_t len)
    {
        if (len > 32)
            return false;

        uint8_t status = read_register(NRF24L01_STATUS);

        if (status & NRF24L01_STATUS_RX_DR)
        {
            uint8_t fifo_status = read_register(NRF24L01_FIFO_STATUS);

            if (!(fifo_status & 0x01))
            { // RX_EMPTY = 0 → có data
                read_payload(data, len);
                write_register(NRF24L01_STATUS, NRF24L01_STATUS_RX_DR);
                return true;
            }
            write_register(NRF24L01_STATUS, NRF24L01_STATUS_RX_DR);
        }
        return false;
    }

    void RP2040Nrf24l01::print_register_map()
    {
        uint8_t tx_addr[5];
        uint8_t rx_addr_0[5];
        uint8_t rx_addr_1[5];
        uint8_t rx_addr_2[5];
        uint8_t rx_addr_3[5];
        uint8_t rx_addr_4[5];
        uint8_t rx_addr_5[5];

        read_register_multi(NRF24L01_TX_ADDR, tx_addr, 5);
        read_register_multi(NRF24L01_RX_ADDR_P0, rx_addr_0, 5);
        read_register_multi(NRF24L01_RX_ADDR_P1, rx_addr_1, 5);
        read_register_multi(NRF24L01_RX_ADDR_P2, rx_addr_2, 5);
        read_register_multi(NRF24L01_RX_ADDR_P3, rx_addr_3, 5);
        read_register_multi(NRF24L01_RX_ADDR_P4, rx_addr_4, 5);
        read_register_multi(NRF24L01_RX_ADDR_P5, rx_addr_5, 5);

        uint8_t config = read_register(NRF24L01_CONFIG);
        uint8_t en_aa = read_register(NRF24L01_EN_AA);
        uint8_t en_rxaddr = read_register(NRF24L01_EN_RXADDR);
        uint8_t setup_aw = read_register(NRF24L01_SETUP_AW);
        uint8_t setup_retr = read_register(NRF24L01_SETUP_RETR);
        uint8_t rf_ch = read_register(NRF24L01_RF_CH);
        uint8_t rf_setup = read_register(NRF24L01_RF_SETUP);
        uint8_t status = read_register(NRF24L01_STATUS);
        uint8_t observe_tx = read_register(NRF24L01_OBSERVE_TX);
        uint8_t rpd = read_register(NRF24L01_RPD);

        uint8_t rx_pw_p0 = read_register(NRF24L01_RX_PW_P0);
        uint8_t rx_pw_p1 = read_register(NRF24L01_RX_PW_P1);
        uint8_t rx_pw_p2 = read_register(NRF24L01_RX_PW_P2);
        uint8_t rx_pw_p3 = read_register(NRF24L01_RX_PW_P3);
        uint8_t rx_pw_p4 = read_register(NRF24L01_RX_PW_P4);
        uint8_t rx_pw_p5 = read_register(NRF24L01_RX_PW_P5);
        uint8_t fifo_status = read_register(NRF24L01_FIFO_STATUS);
        uint8_t dynpd = read_register(NRF24L01_DYNPD);
        uint8_t feature = read_register(NRF24L01_FEATURE);

        

        printf("CONFIG: 0x%02X\n", config);
        printf("EN_AA: 0x%02X\n", en_aa);
        printf("EN_RXADDR: 0x%02X\n", en_rxaddr);
        printf("SETUP_AW: 0x%02X\n", setup_aw);
        printf("SETUP_RETR: 0x%02X\n", setup_retr);
        printf("RF_CH: 0x%02X\n", rf_ch);
        printf("RF_SETUP: 0x%02X\n", rf_setup);
        printf("STATUS: 0x%02X\n", status);
        printf("OBSERVE_TX: 0x%02X\n", observe_tx);
        printf("RPD: 0x%02X\n", rpd);
        printf("RX_PW_P0: 0x%02X\n", rx_pw_p0);
        printf("RX_PW_P1: 0x%02X\n", rx_pw_p1);
        printf("RX_PW_P2: 0x%02X\n", rx_pw_p2);
        printf("RX_PW_P3: 0x%02X\n", rx_pw_p3);
        printf("RX_PW_P4: 0x%02X\n", rx_pw_p4);
        printf("RX_PW_P5: 0x%02X\n", rx_pw_p5);
        printf("FIFO_STATUS: 0x%02X\n", fifo_status);
        printf("DYNPD: 0x%02X\n", dynpd);
        printf("FEATURE: 0x%02X\n", feature);

        printf("TX_ADDR = %02X %02X %02X %02X %02X\n", tx_addr[0], tx_addr[1], tx_addr[2], tx_addr[3], tx_addr[4]);
        printf("RX_ADDR_P0 = %02X %02X %02X %02X %02X\n", rx_addr_0[0], rx_addr_0[1], rx_addr_0[2], rx_addr_0[3], rx_addr_0[4]);
        printf("RX_ADDR_P1 = %02X %02X %02X %02X %02X\n", rx_addr_1[0], rx_addr_1[1], rx_addr_1[2], rx_addr_1[3], rx_addr_1[4]);
        printf("RX_ADDR_P2 = %02X %02X %02X %02X %02X\n", rx_addr_2[0], rx_addr_2[1], rx_addr_2[2], rx_addr_2[3], rx_addr_2[4]);
        printf("RX_ADDR_P3 = %02X %02X %02X %02X %02X\n", rx_addr_3[0], rx_addr_3[1], rx_addr_3[2], rx_addr_3[3], rx_addr_3[4]);
        printf("RX_ADDR_P4 = %02X %02X %02X %02X %02X\n", rx_addr_4[0], rx_addr_4[1], rx_addr_4[2], rx_addr_4[3], rx_addr_4[4]);
        printf("RX_ADDR_P5 = %02X %02X %02X %02X %02X\n\n", rx_addr_5[0], rx_addr_5[1], rx_addr_5[2], rx_addr_5[3], rx_addr_5[4]);
    }
} // namespace aircraft_lib
