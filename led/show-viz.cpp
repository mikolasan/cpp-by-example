#include <array>
#include <chrono>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "Artnet.h"
#include "ipaddress.h"
#include "show_cached.hpp"
#include "raylib.h"

using namespace std::chrono_literals;

const int n_leds_per_universe = 170;
const int max_leds_per_row = 50;
const int led_size = 10;

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
        const std::string& filename,
        std::map<int, std::vector<Color>>* pixels,
        std::mutex* pixels_mutex
    ) : 
        pixels(pixels),
        pixels_mutex(pixels_mutex),
        ShowCached(codename, filename)
    {}

    void send() override {
        sender = std::thread([this](){
            play();
        });
        sender.detach();
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
private:
    using ArtnetReceiver = arx::artnet::Receiver<TestStream>;
    ArtnetReceiver artnet;
    std::thread sender;
    std::mutex *pixels_mutex;
    std::map<int, std::vector<Color>> *pixels;
};

void draw(const std::string& show_name, const std::map<int, std::vector<Color>>& pixels, std::mutex& pixels_mutex) {
    BeginDrawing();

    ClearBackground(BLACK);
    DrawText(show_name.c_str(), 20, 20, 20, LIGHTGRAY);

    int start_x = 10;
    int start_y = 100;
    int current_y = start_y;

    std::lock_guard<std::mutex> lock(pixels_mutex);
    for (auto& [universe_id, colors] : pixels) {
        for (int i = 0; i < colors.size(); ++i) {
            int x = start_x + (i % max_leds_per_row) * led_size;
            int y = current_y + (i / max_leds_per_row) * led_size;
            DrawRectangle(x, y, led_size, led_size, colors[i]);
        }
        current_y += (2 + colors.size() / max_leds_per_row) * led_size;
    }

    EndDrawing();
}

int main(int argc, char const *argv[])
{
    std::string show_file{"test.pcapng"};
    std::string show_name = "music of happy pigs";

    std::mutex pixels_mutex;
    std::map<int, std::vector<Color>> pixels;
    pixels[0] = std::vector<Color>(n_leds_per_universe, RED);
    pixels[1] = std::vector<Color>(n_leds_per_universe, ORANGE);
    pixels[2] = std::vector<Color>(n_leds_per_universe, YELLOW);
    pixels[3] = std::vector<Color>(n_leds_per_universe, WHITE);

    ShowTest* show = new ShowTest(show_name, show_file, &pixels, &pixels_mutex);
    show->send();

    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Art Net viz");
    SetTargetFPS(60);

    while (!WindowShouldClose()){
        draw(show_name, pixels, pixels_mutex);
    }

    CloseWindow();
    return 0;
}