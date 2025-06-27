#include "nrf24l01_interface.h"
#include "esp32_nrf24l01.h"
#include "soft_access_point.h"
#include "socket_communication.h"


#define CE_PIN GPIO_NUM_16
#define CSN_PIN GPIO_NUM_17
#define IRQ_PIN GPIO_NUM_5
#define SCK_PIN GPIO_NUM_18
#define MOSI_PIN GPIO_NUM_23
#define MISO_PIN GPIO_NUM_19

const char *ssid = "ESP32_TX";
const char *password = "12345678";

const char *user_name_tcp = "admin";
const char *password_tcp = "270603";

uint8_t tx_address[5] = {'P', 'L', 'A', 'N', 'E'};
uint8_t payload_len = 32;
uint8_t channel = 20;
uint8_t tx_payload[32];


extern "C" void app_main(void)
{
    aircraft_lib::Esp32Nrf24l01 nrf_tx(CE_PIN, CSN_PIN, IRQ_PIN, SCK_PIN, MOSI_PIN, MISO_PIN);
    nrf_tx.init();
    nrf_tx.activate_features();
    nrf_tx.config_tx_mode(tx_address, payload_len, channel);

    aircraft_lib::wifi_nvs_flash_init();
    aircraft_lib::wifi_init_softap(ssid, password, 1, 1);
    
    aircraft_lib::Socket server_socket(aircraft_lib::SocketType::Stream, aircraft_lib::ProtocolType::Tcp);
    server_socket.bind(inet_addr("0.0.0.0"), 2003);
    server_socket.listen(1);

    char tcp_receive_buffer[128];

    while (1)
    {
        // Chấp nhận kết nối
        aircraft_lib::Socket client_socket = server_socket.accept();        


        while (1)
        {
            int len = client_socket.recv(tcp_receive_buffer, sizeof(tcp_receive_buffer), 0);

            if (len < 0)
            {
                ESP_LOGE("LOG", "recv failed: errno %d", errno);
                break;
            }
            else if (len == 0)
            {
                ESP_LOGW("LOG", "Connection closed");
                break;
            }
            else
            {
                tcp_receive_buffer[len] = 0;
                ESP_LOGI("LOG", "Received: %s", tcp_receive_buffer);

                // Phản hồi lại client (echo)
                client_socket.send(tcp_receive_buffer, len, 0);
            }
        }

        client_socket.close();
        ESP_LOGI("LOG", "Client disconnected");
    }
    
    server_socket.close();

    while (1)
    {
        // char msg[7] = "Hello ";
        // memset(tx_payload, ' ', 32);
        // memcpy(tx_payload, msg, strlen(msg)); // pad bằng space nếu thiếu
        // bool ok = nrf_tx.transmit(tx_payload, payload_len, 100);

        // if (ok)
        //     printf("[TX] Gửi thành công %d\n", i);
        // else
        //     printf("[TX] Gửi thất bại!\n");
        // vTaskDelay(pdMS_TO_TICKS(100));

         vTaskDelay(pdMS_TO_TICKS(100));
    }
}