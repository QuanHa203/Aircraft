#ifndef SOCKET_COMMUNICATION_H
#define SOCKET_COMMUNICATION_H

#include "stdint.h"

#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>

#include "esp_log.h"

namespace aircraft_lib
{
    enum SocketType
    {
        Stream = SOCK_STREAM,
        Dgram = SOCK_DGRAM,
        R = SOCK_RAW
    };

    enum ProtocolType
    {
        IP = IPPROTO_IP,
        Tcp = IPPROTO_TCP,
        Udp = IPPROTO_UDP,
        RawType = IPPROTO_RAW,
        Icmp = IPPROTO_ICMP,
        UpdLite = IPPROTO_UDPLITE
    };

    class Socket
    {
    private:
        const char *TAG = "socket_communication";
        SocketType socket_type_;
        ProtocolType protocol_type_;

        struct sockaddr_in socket_addr_;
        int socket_desc_ = -1;

    public:

        Socket();
        Socket(SocketType socket_type, ProtocolType protocol_type);

        void bind(uint32_t ip_addr, uint16_t port);
        void listen(int backlog);

        Socket accept();
        int send(void *buffer, size_t len, int flags);
        int recv(void *buffer, size_t len, int flags);

        void shutdown();
        void close();

        int get_socket_desc_();

        ~Socket() = default;
    };
}
#endif