#include <iostream>
#include <string>
#include <string_view>

#include "network.h"

int main(int argc, char const *argv[])
{
    std::string hostname{"192.168.0.4"};
    uint16_t port = 9000;

    const char data[] = "Jane Doe";
    size_t length = 8;
    std::string_view msg{&data[0], length};

    UdpSender udp;
    udp.set_destination(hostname, port);
    int n_bytes = udp.send(msg);
    std::cout << n_bytes << " bytes sent" << std::endl;

    return 0;
}