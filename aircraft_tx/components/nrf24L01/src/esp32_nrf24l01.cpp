#include "esp32_nrf24l01.h"

// Static field
spi_device_handle_t Esp32Nrf24l01::nrf24l01_spi_ = nullptr;

// Contructor
Esp32Nrf24l01::Esp32Nrf24l01(gpio_num_t ce_pin, gpio_num_t csn_pin, gpio_num_t irq_pin, gpio_num_t sck_pin, gpio_num_t mosi_pin, gpio_num_t miso_pin)
    : ce_pin_(ce_pin), csn_pin_(csn_pin), irq_pin_(irq_pin), sck_pin_(sck_pin), mosi_pin_(mosi_pin), miso_pin_(miso_pin) {}

// Private method
void Esp32Nrf24l01::set_ce_pin_high()
{
    gpio_set_level(ce_pin_, 1);
}
void Esp32Nrf24l01::set_ce_pin_low()
{
    gpio_set_level(ce_pin_, 0);
}
void Esp32Nrf24l01::set_csn_pin_high()
{
    gpio_set_level(csn_pin_, 1);
}
void Esp32Nrf24l01::set_csn_pin_low()
{
    gpio_set_level(csn_pin_, 0);
}

void Esp32Nrf24l01::clear_interrupt_flags()
{
    write_register(NRF24L01_STATUS,
                   NRF24L01_STATUS_RX_DR | NRF24L01_STATUS_TX_DS | NRF24L01_STATUS_MAX_RT);
}

// Public method
uint8_t Esp32Nrf24l01::read_register(uint8_t reg)
{
    uint8_t cmd = NRF24L01_CMD_R_REGISTER | (reg & 0x1F);
    uint8_t tx_buf[2] = {cmd, NRF24L01_CMD_NOP};
    uint8_t rx_buf[2];

    set_csn_pin_low();
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 16;
    t.tx_buffer = tx_buf;
    t.rx_buffer = rx_buf;

    esp_err_t ret = spi_device_transmit(nrf24l01_spi_, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGE("NRF24", "SPI transmit failed: %s", esp_err_to_name(ret));
    }
    set_csn_pin_high();

    return rx_buf[1];
}

void Esp32Nrf24l01::write_register(uint8_t reg, uint8_t data)
{
    uint8_t cmd = NRF24L01_CMD_W_REGISTER | (reg & 0x1F);
    uint8_t tx_buf[2] = {cmd, data};

    set_csn_pin_low();
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 16;
    t.tx_buffer = tx_buf;
    t.rx_buffer = NULL;

    esp_err_t ret = spi_device_transmit(nrf24l01_spi_, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGE("NRF24", "SPI transmit failed: %s", esp_err_to_name(ret));
    }
    set_csn_pin_high();
}

void Esp32Nrf24l01::read_register_multi(uint8_t reg, uint8_t *data, size_t len)
{
    if (len > 31)
        return;

    uint8_t cmd = NRF24L01_CMD_R_REGISTER | (reg & 0x1F);
    uint8_t tx[1 + len];
    uint8_t rx[1 + len];
    memset(tx, NRF24L01_CMD_NOP, sizeof(tx));
    tx[0] = cmd;

    set_csn_pin_low();
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = (1 + len) * 8,
    t.tx_buffer = tx;
    t.rx_buffer = rx;

    esp_err_t ret = spi_device_transmit(nrf24l01_spi_, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGE("NRF24", "SPI transmit failed: %s", esp_err_to_name(ret));
    }
    set_csn_pin_high();

    memcpy(data, &rx[1], len);
}
void Esp32Nrf24l01::write_register_multi(uint8_t reg, uint8_t *data, size_t len)
{
    uint8_t cmd = NRF24L01_CMD_W_REGISTER | (reg & 0x1F);
    uint8_t tx_buf[1 + len];
    tx_buf[0] = cmd;
    memcpy(&tx_buf[1], data, len);

    set_csn_pin_low();
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = (1 + len) * 8;
    t.tx_buffer = tx_buf;
    t.rx_buffer = NULL;

    esp_err_t ret = spi_device_transmit(nrf24l01_spi_, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGE("NRF24", "SPI transmit failed: %s", esp_err_to_name(ret));
    }
    set_csn_pin_high();
}

void Esp32Nrf24l01::read_payload(uint8_t *data, size_t len)
{
    if (len > 32)
        len = 32;

    uint8_t tx_buf[1 + len];
    uint8_t rx_buf[1 + len];

    tx_buf[0] = NRF24L01_CMD_R_RX_PAYLOAD;
    memset(&tx_buf[1], NRF24L01_CMD_NOP, len);

    set_csn_pin_low();
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = (1 + len) * 8;
    t.tx_buffer = tx_buf;
    t.rx_buffer = rx_buf;

    esp_err_t ret = spi_device_transmit(nrf24l01_spi_, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGE("NRF24", "SPI transmit failed: %s", esp_err_to_name(ret));
    }
    set_csn_pin_high();

    memcpy(data, &rx_buf[1], len);
}

void Esp32Nrf24l01::write_payload(uint8_t *data, size_t len)
{
    if (len > 32)
        len = 32;

    uint8_t tx_buf[1 + len];
    tx_buf[0] = NRF24L01_CMD_W_TX_PAYLOAD;
    memcpy(&tx_buf[1], data, len);

    set_csn_pin_low();
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = (1 + len) * 8;
    t.tx_buffer = tx_buf;
    t.rx_buffer = NULL;

    esp_err_t ret = spi_device_transmit(nrf24l01_spi_, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGE("NRF24", "SPI transmit failed: %s", esp_err_to_name(ret));
    }
    set_csn_pin_high();
}

void Esp32Nrf24l01::init()
{
    esp_err_t ret;

    // Configure CE and CSN pin
    gpio_reset_pin(csn_pin_);
    gpio_reset_pin(ce_pin_);
    gpio_set_direction(csn_pin_, GPIO_MODE_OUTPUT);
    gpio_set_direction(ce_pin_, GPIO_MODE_OUTPUT);

    gpio_set_level(csn_pin_, 1); // Disconnect default SPI
    gpio_set_level(ce_pin_, 0);  // Disable TX/RX mode

    // 2. Configure SPI bus
    spi_bus_config_t spi_bus_config;
    memset(&spi_bus_config, 0, sizeof(spi_bus_config));
    spi_bus_config.mosi_io_num = mosi_pin_;
    spi_bus_config.miso_io_num = miso_pin_;
    spi_bus_config.sclk_io_num = sck_pin_;
    spi_bus_config.quadwp_io_num = -1;
    spi_bus_config.quadhd_io_num = -1;

    ret = spi_bus_initialize(SPI2_HOST, &spi_bus_config, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK)
        ESP_LOGE("NRF", "SPI bus init failed: %s", esp_err_to_name(ret));

    // 3. Add SPI device (nRF24L01)
    spi_device_interface_config_t device_config;
    memset(&device_config, 0, sizeof(device_config));
    device_config.mode = 0;
    device_config.clock_speed_hz = 2 * 1000 * 1000; // 2 MHz
    device_config.spics_io_num = -1;                // manual control CSN
    device_config.queue_size = 1;

    ret = spi_bus_add_device(SPI2_HOST, &device_config, &nrf24l01_spi_);
    if (ret != ESP_OK)
        ESP_LOGE("NRF", "Add device failed: %s", esp_err_to_name(ret));

    uint8_t activate_cmd[2] = {0x50, 0x73};
    spi_transaction_t t = {};
    t.length = 16;
    t.tx_buffer = activate_cmd;
    spi_device_transmit(nrf24l01_spi_, &t);

    // Không bật FEATURE nếu chưa cần
    write_register(NRF24L01_FEATURE, 0x00);
}

void Esp32Nrf24l01::flush_tx()
{
    uint8_t cmd = NRF24L01_CMD_FLUSH_TX;
    set_csn_pin_low();
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.rx_buffer = NULL;

    esp_err_t ret = spi_device_transmit(nrf24l01_spi_, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGE("NRF24", "SPI transmit failed: %s", esp_err_to_name(ret));
    }
    set_csn_pin_high();
}

void Esp32Nrf24l01::flush_rx()
{
    uint8_t cmd = NRF24L01_CMD_FLUSH_RX;
    set_csn_pin_low();
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;

    esp_err_t ret = spi_device_transmit(nrf24l01_spi_, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGE("NRF24", "SPI transmit failed: %s", esp_err_to_name(ret));
    }
    set_csn_pin_high();
}

void Esp32Nrf24l01::config_tx_mode(uint8_t *tx_addr, uint8_t payload_len, uint8_t channel)
{
    set_ce_pin_low();
    
    write_register_multi(NRF24L01_TX_ADDR, tx_addr, 5);
    write_register_multi(NRF24L01_RX_ADDR_P0, tx_addr, 5); // Quan trọng để gửi ACK (nếu bật)

    write_register(NRF24L01_EN_AA, 0x01);     // Enable Auto-ACK pipe 0
    write_register(NRF24L01_EN_RXADDR, 0x01); // Enable pipe 0

    write_register(NRF24L01_SETUP_RETR, 0x33); // Retry 3 lần, delay 1000us

    write_register(NRF24L01_RF_CH, channel);
    write_register(NRF24L01_RF_SETUP, 0x06); // 1Mbps, 0dBm

    write_register(NRF24L01_SETUP_AW, 0x03); // 5-byte address width

    write_register(NRF24L01_RX_PW_P0, payload_len);

    write_register(NRF24L01_DYNPD, 0x00);
    write_register(NRF24L01_FEATURE, 0x00);

    write_register(NRF24L01_CONFIG, 0x0A); // PWR_UP = 1, PRIM_RX = 0

    vTaskDelay(pdMS_TO_TICKS(5)); // Wait for power up

    clear_interrupt_flags();
    flush_tx();
}

void Esp32Nrf24l01::config_rx_mode(uint8_t *rx_addr, uint8_t payload_len, uint8_t channel, uint8_t pipe)
{
    set_ce_pin_low();
    
    // Cấu hình địa chỉ pipe
    write_register_multi(NRF24L01_RX_ADDR_P0 + pipe, rx_addr, 5);

    // Enable pipe
    write_register(NRF24L01_EN_RXADDR, (1 << pipe));
    write_register(NRF24L01_EN_AA, (1 << pipe)); // Tạm bật để test

    write_register(NRF24L01_SETUP_AW, 0x03);
    write_register(NRF24L01_RF_CH, channel);
    write_register(NRF24L01_RF_SETUP, 0x06);
    write_register(NRF24L01_RX_PW_P0 + pipe, payload_len);

    // Tắt các tính năng nâng cao
    write_register(NRF24L01_DYNPD, 0x00);
    write_register(NRF24L01_FEATURE, 0x00);

    // Cấu hình RX mode
    write_register(NRF24L01_CONFIG, 0x0B);
    vTaskDelay(pdMS_TO_TICKS(10));

    // Debug xác minh
    printf("CONFIG set: 0x%02X\n", read_register(NRF24L01_CONFIG));

    clear_interrupt_flags();
    flush_rx();

    set_ce_pin_high();
}

bool Esp32Nrf24l01::transmit(uint8_t *data, size_t len, uint32_t timeoutMs)
{
    write_register(NRF24L01_STATUS, NRF24L01_STATUS_TX_DS | NRF24L01_STATUS_MAX_RT | NRF24L01_STATUS_RX_DR);

    // 1. Flush FIFO TX trước khi gửi mới
    flush_tx();

    // 2. Ghi payload vào FIFO TX
    write_payload(data, len);

    // 3. Pulse CE >=10µs để gửi
    set_ce_pin_high();
    esp_rom_delay_us(15); // Gửi 15µs là đủ
    set_ce_pin_low();

    // 4. Poll STATUS để kiểm tra kết quả
    uint32_t start_tick = xTaskGetTickCount();
    while ((xTaskGetTickCount() - start_tick) < pdMS_TO_TICKS(timeoutMs))
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

        vTaskDelay(pdMS_TO_TICKS(1)); // Nhẹ nhàng CPU
    }

    // Hết timeout
    return false;
}

bool Esp32Nrf24l01::receive(uint8_t *data, size_t len)
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

void Esp32Nrf24l01::activate_features()
{
    uint8_t activate_cmd[2] = {0x50, 0x73};
    spi_transaction_t t = {};
    memset(&t, 0, sizeof(t));
    t.length = 16;
    t.tx_buffer = activate_cmd;
    t.rx_buffer = NULL;

    set_csn_pin_low();
    esp_err_t ret = spi_device_transmit(nrf24l01_spi_, &t);
    set_csn_pin_high();

    if (ret != ESP_OK)
        ESP_LOGE("NRF24", "ACTIVATE failed: %s", esp_err_to_name(ret));
}

void Esp32Nrf24l01::print_register_map()
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