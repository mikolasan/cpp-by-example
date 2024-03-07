#include <iomanip>
#include <iostream>
#include <string>

#include "network.h"
#include "pcap_stream.h"

int main(int argc, char const *argv[])
{
    if (argc != 4) {
        std::cerr << "this program needs (hostname, port, file name) as input arguments" << std::endl;
        return 1;
    }

    std::string hostname(argv[1]);
    uint16_t port = std::stoi(argv[2]); // must 6454
    std::string fname(argv[3]);

    try {
        UdpSender udp;
        udp.set_destination(hostname, port);

        PcapStream stream{fname};

        do {
            const size_t size = stream.parsePacket();
            if (size == 0) continue;
            stream.wait();
            int n_bytes = udp.send(stream.getData());
            // std::cout << n_bytes << " bytes sent" << std::endl;

        } while (stream.hasPackets());
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
