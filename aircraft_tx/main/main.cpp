#include "nrf24l01_interface.h"
#include "esp32_nrf24l01.h"
#include "soft_access_point.h"

#define CE_PIN GPIO_NUM_16
#define CSN_PIN GPIO_NUM_17
#define IRQ_PIN GPIO_NUM_5
#define SCK_PIN GPIO_NUM_18
#define MOSI_PIN GPIO_NUM_23
#define MISO_PIN GPIO_NUM_19

const char* ssid = "ESP32_TX";
const char* password = "12345678";

uint8_t tx_address[5] = {'P', 'L', 'A', 'N', 'E'};
uint8_t payload_len = 32;
uint8_t channel = 20;
uint8_t tx_payload[32];

extern "C" void app_main(void)
{
    Esp32Nrf24l01 nrf_tx(CE_PIN, CSN_PIN, IRQ_PIN, SCK_PIN, MOSI_PIN, MISO_PIN);
    nrf_tx.init();
    nrf_tx.activate_features();
    nrf_tx.config_tx_mode(tx_address, payload_len, channel);

    int i = 0;
 
    wifi_nvs_flash_init();
    wifi_init_softap(ssid, password, 1, 1);
    while (1)
    {
        // char msg[7] = "Hello ";
        // msg[6] = (char)i;
        // memset(tx_payload, ' ', 32);
        // memcpy(tx_payload, msg, strlen(msg)); // pad bằng space nếu thiếu
        // bool ok = nrf_tx.transmit(tx_payload, payload_len, 100);

        // if (ok)
        //     printf("[TX] Gửi thành công %d\n", i);
        // else
        //     printf("[TX] Gửi thất bại!\n");
        // vTaskDelay(pdMS_TO_TICKS(100));
        // i++;

         vTaskDelay(pdMS_TO_TICKS(100));
    }
    
}