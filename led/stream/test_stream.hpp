#include <cstring> // memcpy
#include <optional>
#include <stdint.h>
#include <string>

#include "ipaddress.h"

class TestStream {
public:
    TestStream() {}

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
    
private:
    std::optional<std::string> data;
};