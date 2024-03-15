#include <cstring> // memcpy
#include <optional>
#include <stdint.h>
#include <string>

#include "ipaddress.h"

class SequentialStream {
public:
    SequentialStream() {}

    // receiver
    void open(const std::string& data) {
        this->data = data;
    }
    bool hasPackets() {
        return data.has_value();
    }
    size_t parsePacket() {
        if (data) {
            return data.value().size();
        } else {
            return 0;
        }
    }
    
    template<typename C>
    void read(C d, size_t size) {
        if (!data) return;
        std::memcpy(d, data.value().data(), size);
        data.reset();
    }

    IPAddress remoteIP() const { return IPAddress(0U); }
    uint16_t remotePort() const { return 0; }
    
    // sender
    void begin(uint16_t port) {
        // does nothing
    }
    
    void beginPacket(const char* hostname_c, uint16_t port) {
        // does nothing
    }
    
    void write(const uint8_t* data, size_t size) {
        packet = std::string_view(reinterpret_cast<const char*>(data), size);
    }

    void endPacket() {
        if (auto p = udp_sender.lock()) {
            try {
                int n_bytes = p->send(packet);
            } catch (const network_error& e) {
                std::cerr << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << e.what() << '\n';
            }
        } else {
            std::cout << "\tdo_send() is unable to lock weak_ptr<>\n";
        }
    }

public:
    std::weak_ptr<UdpSender> udp_sender;
private:
    std::optional<std::string> data;
    std::string_view packet;
};