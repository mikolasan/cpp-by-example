#include <stdexcept>

#include <arpa/inet.h> // htons, inet_addr
#include <errno.h> // errno
#include <sys/socket.h> // socket, sendto
#include <unistd.h> // close

#include "udp_sender.h"

UdpSender::UdpSender() {
    sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        throw network_error("socket", errno);
    }
    sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = 10000; // random port
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    int result = ::bind(sock, reinterpret_cast<sockaddr*>(&local), sizeof(local));
    if (result == -1) {
        throw network_error("bind", errno);
    }
}

UdpSender::~UdpSender() noexcept(false) {
    if (::close(sock) == -1) {
        throw network_error("close", errno);
    }
}

void UdpSender::set_destination(const std::string& hostname, uint16_t port) {
    dest = destanation(hostname, port);
}

void UdpSender::enable_broadcast() {
    int enable_flag = 1;
    // sendto() shall fail if the SO_BROADCAST option is not set for the socket
    int result = ::setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &enable_flag, sizeof(enable_flag));
    if (result == -1) {
        throw network_error("setsockopt", errno);
    }
}

size_t UdpSender::send(const std::string_view& data) const {
    ssize_t n_bytes = ::sendto(sock, data.data(), data.length(), 0, reinterpret_cast<const sockaddr*>(&dest), sizeof(dest));
    if (n_bytes == -1) {
        throw network_error("sendto", errno);
    }
    return n_bytes;
}

size_t UdpSender::send_to(const std::string& hostname, uint16_t port, const std::string_view& data) const {
    sockaddr_in tmp_dest = destanation(hostname, port);
    ssize_t n_bytes = ::sendto(sock, data.data(), data.length(), 0, reinterpret_cast<sockaddr*>(&tmp_dest), sizeof(tmp_dest));
    if (n_bytes == -1) {
        throw network_error("sendto", errno);
    }
    return n_bytes;
}

sockaddr_in UdpSender::destanation(const std::string& hostname, uint16_t port) const {
    sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    dest.sin_addr.s_addr = inet_addr(hostname.c_str());
    return dest;
}