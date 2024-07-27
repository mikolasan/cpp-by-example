#include <iostream>
#include <string>
#include <string_view>

#include <arpa/inet.h> // htons, inet_addr
#include <netinet/in.h> // sockaddr_in
#include <sys/types.h> // uint16_t
#include <sys/socket.h> // socket, sendto
#include <unistd.h> // close

void send_data(const std::string& hostname, uint16_t port, const std::string_view& data) {
    int sock = ::socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in destination;
    destination.sin_family = AF_INET;
    destination.sin_port = htons(port);
    destination.sin_addr.s_addr = inet_addr(hostname.c_str());

    int n_bytes = ::sendto(sock, data.data(), data.length(), 0, reinterpret_cast<sockaddr*>(&destination), sizeof(destination));
    std::cout << n_bytes << " bytes sent" << std::endl;
    ::close(sock);
}

int main(int argc, char const *argv[])
{
    std::string hostname{"192.168.0.4"};
    uint16_t port = 9000;

    const char data[] = "Jane Doe";
    size_t length = 8;
    std::string_view msg{&data[0], length};

    send_data(hostname, port, msg);

    return 0;
}