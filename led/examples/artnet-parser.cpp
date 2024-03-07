#include <iomanip>
#include <iostream>

#include "Artnet.h"
#include "pcap_stream.h"

enum READER_ERROR {
    OK = 0,
    BAD_ARGUMENTS,
    OPEN_FILE,
    READ_FILE,
};

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        std::cerr << "this program needs file name as input argument" << std::endl;
        return READER_ERROR::BAD_ARGUMENTS;
    }

    std::string fname(argv[1]);
    try {
        using ArtnetReceiver = arx::artnet::Receiver<PcapStream>;
        ArtnetReceiver artnet;
        artnet.open(fname);
        artnet.subscribe([](const uint32_t universe, const uint8_t* data, const uint16_t size) -> void {
            std::cout << "UNIVERSE " << universe
                << " size " << std::dec << size << std::endl;
            for (int i = 0; i < size / 3; ++ i) {
                std::cout << "[ ";
                std::cout << static_cast<int>(data[3*i]) << " ";
                std::cout << static_cast<int>(data[3*i + 1]) << " ";
                std::cout << static_cast<int>(data[3*i + 2]) << " ";
                std::cout << "] ";
            }
            std::cout << std::endl;
        });
        do {
            artnet.parse();
        } while (artnet.hasPackets());
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return READER_ERROR::OPEN_FILE;
    }

    return READER_ERROR::OK;
}
