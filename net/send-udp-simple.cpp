#include <iostream>
#include <string>

#include <arpa/inet.h> // htons, inet_addr
#include <netinet/in.h> // sockaddr_in
#include <sys/types.h> // uint16_t
#include <sys/socket.h> // socket, sendto
#include <unistd.h> // close

int main(int argc, char const *argv[])
{
    std::string hostname{"192.168.0.4"};
    uint16_t port = 9000;

    int sock = ::socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in destination;
    destination.sin_family = AF_INET;
    destination.sin_port = htons(port);
    destination.sin_addr.s_addr = inet_addr(hostname.c_str());

    std::string msg = "Jane Doe";
    int n_bytes = ::sendto(sock, msg.c_str(), msg.length(), 0, reinterpret_cast<sockaddr*>(&destination), sizeof(destination));
    std::cout << n_bytes << " bytes sent" << std::endl;
    ::close(sock);

    return 0;
}