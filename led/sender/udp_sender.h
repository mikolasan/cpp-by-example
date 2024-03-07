#pragma once

#include <netinet/in.h> // sockaddr_in
#include <stdint.h> // uint16_t
#include <string>
#include <string_view>

class network_error : public std::exception {
public:
    network_error(const std::string& function_name, int code);
    const char* what() const noexcept override;
private:
    std::string message;
};

class UdpSender {
public:
    UdpSender();
    ~UdpSender() noexcept(false);

    void set_destination(const std::string& hostname, uint16_t port);
    void enable_broadcast();
    
    size_t send(const std::string_view& data) const;
    size_t send_to(const std::string& hostname, uint16_t port, const std::string_view& data) const;

protected:
    sockaddr_in destanation(const std::string& hostname, uint16_t port) const;

private:
    int sock;
    sockaddr_in dest;
};