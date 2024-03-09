#include <condition_variable>
#include <map>
#include <mutex>
#include <stdint.h>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "Artnet.h"
#include "show_cached.hpp"
#include "test_stream.hpp"

class TestShow : public ShowCached {
public:
    TestShow(
        const std::string& codename,
        const std::string& filename
    ) : 
        pixels(nullptr),
        pixels_mutex(nullptr),
        ShowCached(codename, filename)
    {}

    void send() override {
        throw std::runtime_error("TestShow send not implemented");
        play();
    }

    std::string send_blocking() {
        play();
        return codename;
    }
    void send_async() {
        std::thread t([this](){
            play();
        });
        t.detach();
    }

protected:
    size_t do_send(const std::string_view& data) override {
        std::string data_copy(data.data(), data.size());
        artnet.open(data_copy);

        artnet.subscribe([this](const uint32_t universe, const uint8_t* data, const uint16_t size) -> void {
            std::lock_guard<std::mutex> lock(*pixels_mutex);
            for (int i = 0; i < size / 3; ++ i) {
                auto c = Color(data[3*i], data[3*i + 1], data[3*i + 2], 255);
                (*pixels)[universe][i] = c;
            }
        });
        
        do {
            artnet.parse();
        } while (artnet.hasPackets());

        return 0;
    }

public:
    std::condition_variable cv;
    std::mutex *pixels_mutex;
    std::map<int, std::vector<Color>> *pixels;
private:
    using ArtnetReceiver = arx::artnet::Receiver<TestStream>;
    ArtnetReceiver artnet;
    std::thread sender;
};
