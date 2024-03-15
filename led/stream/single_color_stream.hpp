#include <stdint.h>
#include <string>
#include <string_view>

#include "network.h"

const uint16_t port = 6454;

class SingleColorStream {
public:
    SingleColorStream() {}
    
    void begin(uint16_t port) {
        // does nothing
    }
    
    void beginPacket(const char* hostname_c, uint16_t port) {
        std::string hostname = std::string(hostname_c);
        udp.set_destination(hostname, port);
        if (hostname.ends_with("255")) {
            udp.enable_broadcast();
        }
    }
    
    void write(const uint8_t* data, size_t size) {
        packet = std::string_view(reinterpret_cast<const char*>(data), size);
    }

    void endPacket() {
        int n_bytes = udp.send(packet);
    }

private:
    UdpSender udp;
    std::string_view packet;
};
