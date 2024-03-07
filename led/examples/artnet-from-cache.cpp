#include <iomanip>
#include <iostream>

#include "Artnet.h"
#include "ipaddress.h"
#include "show_cached.hpp"

enum READER_ERROR {
    OK = 0,
    BAD_ARGUMENTS,
    OPEN_FILE,
    READ_FILE,
};

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

using ArtnetSender = arx::artnet::Sender<TestStream>;

class ShowTest : public ShowCached {
public:
    ShowTest(
        const std::string& codename,
        const std::string& filename
    ) : ShowCached(codename, filename)
    {}

    void send() override {
        play();
    }
protected:
    size_t do_send(const std::string_view& data) override {
        using ArtnetReceiver = arx::artnet::Receiver<TestStream>;
        ArtnetReceiver artnet;

        std::string data_copy(data.data(), data.size());
        artnet.open(data_copy);
        artnet.subscribe([](const uint32_t universe, const uint8_t* data, const uint16_t size) -> void {
            if (universe != 0) return;
            
            std::cout << "UNIVERSE " << universe
                << " size " << std::dec << (size / 3) << std::endl;
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

        return 0;
    }
};

int main(int argc, char const *argv[])
{
    // if (argc != 2) {
    //     std::cerr << "this program needs file name as input argument" << std::endl;
    //     return READER_ERROR::BAD_ARGUMENTS;
    // }
    // std::string show_file(argv[1]);
    std::string show_file{"test.pcapng"};

    ShowTest* show = new ShowTest("test", show_file);
    show->send();

    return READER_ERROR::OK;
}
