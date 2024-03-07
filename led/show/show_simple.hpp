#include <chrono>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>

#include "show_abstract.h"
#include "Artnet.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Duration = std::chrono::milliseconds;
using ArtnetSender = arx::artnet::Sender<SingleColorStream>;

class ShowSimple : public AbstractShow {
public:
    ShowSimple(
        const std::string& codename,
        const std::string& section,
        const std::tuple<int, int, int>& color_tuple,
        const std::string& frequency
    ) :
        AbstractShow(codename, section),
        color_tuple(color_tuple),
        frequency(frequency)
    {}

    void send_to(const std::string& hostname) override {
        show_playing = true;
        ArtnetSender artnet;
        artnet.set_physical(1);
        artnet.begin();
        
        const size_t n_leds = 170;
        const size_t colors_per_led = 3;
        size_t size = n_leds * colors_per_led;
        uint8_t black_data[size];
        for (size_t i = 0; i < size; ++i) {
            black_data[i] = 0;
        }
        
        uint8_t data[size];
        for (size_t i = 0; i < n_leds; ++i) {
            data[colors_per_led * i] = std::get<0>(color_tuple);
            data[colors_per_led * i + 1] = std::get<1>(color_tuple);
            data[colors_per_led * i + 2] = std::get<2>(color_tuple);
        }
        
        using namespace std::chrono_literals;
        bool show_black = false;
        bool send_this_cycle = true;
        TimePoint light_on = Clock::now();

        do {
            
            TimePoint now = Clock::now();
            auto duration = std::chrono::duration_cast<Duration>(now - light_on).count();
            if (frequency == "constant") {
                send_this_cycle = true;
            } else if (frequency == "slow") {
                if (duration > 500) {
                    show_black = !show_black;
                    send_this_cycle = true;
                }
            } else if (frequency == "fast") {
                if (duration > 250) {
                    show_black = !show_black;
                    send_this_cycle = true;
                }
            }
            
            if (send_this_cycle) {
                // std::cout << "send color "
                //     << "(" << std::get<0>(color_tuple)
                //     << "," << std::get<1>(color_tuple)
                //     << "," << std::get<2>(color_tuple)
                //     << ")" << std::endl;
                for (uint8_t universe = 0; universe < 4; ++universe) {
                    artnet.send(hostname, universe, show_black ? black_data : data, size);
                }
                artnet.inc_sequence();
                send_this_cycle = false;
                light_on = Clock::now();
            }

            std::this_thread::sleep_for(5ms);

        } while (show_playing);
    }

private:
    std::tuple<int, int, int> color_tuple;
    std::string frequency;
};
