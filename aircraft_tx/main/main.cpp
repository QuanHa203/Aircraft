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

const uint8_t payload_len = 8;
const uint8_t channel = 110;

uint8_t tx_address[5] = {'P', 'L', 'A', 'N', 'E'};
uint8_t tx_payload[payload_len];

QueueHandle_t packet_queue;

aircraft_lib::Esp32Nrf24l01 esp32_rf_tx(CE_PIN, CSN_PIN, IRQ_PIN, SCK_PIN, MOSI_PIN, MISO_PIN);
INrf24l01 *rf_tx = &esp32_rf_tx;

aircraft_lib::Socket server_socket(aircraft_lib::SocketType::Stream, aircraft_lib::ProtocolType::Tcp);

void handle_tx_sender_task(void *pvParameters);
void handle_socket_task(void *pvParameters);

extern "C" void app_main(void)
{
    // Init WiFi Soft Access Point
    aircraft_lib::wifi_nvs_flash_init();
    aircraft_lib::wifi_init_softap(ssid, password, 1, 1);

    packet_queue = xQueueCreate(4, sizeof(nrf24l01_payload_packet));
    xTaskCreatePinnedToCore(handle_tx_sender_task, "handle_tx_sender_task", 4096, NULL, 4, NULL, 0);
    xTaskCreatePinnedToCore(handle_socket_task, "handle_socket_task", 4096, NULL, 4, NULL, 1);
}

void handle_socket_task(void *pvParameters)
{
    // Init Tcp Server // Listen in port 2003
    server_socket.bind(inet_addr("0.0.0.0"), 2003);
    server_socket.listen(1);
    // uint8_t tcp_receive_buffer[payload_len];

    uint8_t buffer[32];
    char user_name[32] = "";
    char pass[32] = "";
    int size;

    while (1)
    {
        aircraft_lib::Socket client_socket = server_socket.accept();

        while (1)
        {
            /* Check login */
            size = client_socket.recv(buffer, 32, 0);

            // Get UserName + Password from client
            for (size_t i = 0; i < size; i++)
            {
                user_name[i] = buffer[i];
                if (buffer[i] == '\n')
                {
                    i++;
                    size_t j = 0;
                    while (j + i < size)
                    {
                        pass[j] = buffer[i + j];
                        j++;
                    }

                    user_name[i - 1] = '\0'; // Override \n -> \0
                    pass[j] = '\0';
                    break;
                }
            }

            ESP_LOGI("LOG", "UserName: \"%.*s\"\n", sizeof(user_name), user_name);
            ESP_LOGI("LOG", "Password: \"%.*s\"\n", sizeof(pass), pass);

            // UserName + Password correct -> send 0x01
            if (strcmp(user_name, user_name_tcp) == 0 && strcmp(pass, password_tcp) == 0)
            {
                ESP_LOGI("LOG", "Login success");
                client_socket.send((uint8_t[]){0x01}, 1, 0);

                while (1)
                {
                    size = client_socket.recv(tx_payload, payload_len, 0);
                    // size = client_socket.recv(tcp_receive_buffer, sizeof(tcp_receive_buffer), 0);

                    if (size < 0)
                    {
                        ESP_LOGE("LOG", "recv failed: errno %d", errno);
                        break;
                    }
                    else if (size == 0)
                    {
                        ESP_LOGW("LOG", "Connection closed");
                        break;
                    }
                }
                break;
            }
            else // UserName + Password incorrect -> send 0x00
            {
                ESP_LOGI("LOG", "Login fail");
                client_socket.send((uint8_t[]){0x00}, 1, 0);
            }
        }

        client_socket.close();
        ESP_LOGI("LOG", "Client disconnected");        
    }
}

void handle_tx_sender_task(void *pvParameters)
{
    // Init nRF24
    rf_tx->init();
    rf_tx->activate_features();
    rf_tx->config_tx_mode(tx_address, payload_len, channel);
    uint8_t frame_index = 0;

    while (1)
    {        
        tx_payload[0] = frame_index++;
        ESP_LOGI("LOG", "Sending to rx: \"%u %u %u %u %u %u %u %u\"\n", tx_payload[0], tx_payload[1], tx_payload[2], tx_payload[3], tx_payload[4], tx_payload[5], tx_payload[6], tx_payload[7]);
        rf_tx->transmit(tx_payload, payload_len, 20);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
