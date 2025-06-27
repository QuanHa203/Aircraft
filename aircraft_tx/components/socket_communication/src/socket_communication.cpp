#include "socket_communication.h"

namespace aircraft_lib
{
    Socket::Socket() {}
    Socket::Socket(SocketType socket_type, ProtocolType protocol_type) : socket_type_(socket_type), protocol_type_(protocol_type) {}

    void Socket::bind(uint32_t ip_addr, uint16_t port)
    {
        // Tạo socket
        socket_desc_ = socket(AF_INET, socket_type_, protocol_type_);
        if (socket_desc_ < 0)
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);

        // Gán địa chỉ IP và port
        socket_addr_.sin_family = AF_INET;
        socket_addr_.sin_port = htons(port);
        socket_addr_.sin_addr.s_addr = htonl(ip_addr);        

        int err = ::bind(socket_desc_, (struct sockaddr *)&socket_addr_, sizeof(socket_addr_));

        if (err != 0)
        {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            close();
        }
    }

    void Socket::listen(int backlog)
    {
        // Bắt đầu lắng nghe
        int err = ::listen(socket_desc_, backlog);
        if (err != 0)
        {
            ESP_LOGE(TAG, "Error during listen: errno %d", errno);
            close();
        }

        ESP_LOGI(TAG, "TCP Server listening on port %d", socket_addr_.sin_port);
    }

    Socket Socket::accept()
    {
        Socket client_socket(SocketType::Stream, ProtocolType::Tcp);
        
        struct sockaddr_in socket_addr;
        socklen_t socket_addr_len = sizeof(socket_addr);
        
        int client_socket_desc = ::accept(socket_desc_, (struct sockaddr *)&socket_addr, &socket_addr_len);
        
        if (client_socket_desc < 0)
        {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            return client_socket;
        }
        
        char client_addr_str[128];
        inet_ntoa_r(socket_addr.sin_addr, client_addr_str, sizeof(client_addr_str) - 1);
        ESP_LOGI(TAG, "Client connected: %s", client_addr_str);
        
        client_socket.socket_addr_ = socket_addr;
        client_socket.socket_desc_ = client_socket_desc;

        return client_socket;
    }

    int Socket::send(void *buffer, size_t len, int flags)
    {
        return ::send(socket_desc_, buffer, len, flags);
    }

    int Socket::recv(void *buffer, size_t len, int flags)
    {
        return ::recv(socket_desc_, buffer, len, flags);
    }
    
    void Socket::shutdown()
    {
        ::shutdown(socket_desc_, SHUT_WR);
    }

    void Socket::close()
    {
        ::close(socket_desc_);
    }

}
