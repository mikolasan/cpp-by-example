#pragma once

#include <atomic>
#include <iostream>
#include <iterator> // distance
#include <list>
#include <tuple>

#include "Artnet.h"
#include "pcap_stream.h"


const int universe_max_size = 170; // 510 / 3
using RGBData = std::array<CRGB, universe_max_size>;

std::vector<int> data_pins = {4, 5, 6, 7}; // array index = universe id, array value - data pin
// RGB_ORDER = RGB

class ShowLed {
public:
    ShowLed(const std::string& codename, const std::string& filename) :
        codename(codename),
        filename(filename),
        background(false),
        loop(false),
        ignore_stop(false),
        show_playing(false),
        force_stopped(false)
    {}

    void cache() {
        const auto copy = [](std::string_view src) -> std::string_view {
            using Char = std::string_view::value_type;
            Char* const dest = new Char[src.size()];
            src.copy(dest, src.size());
            return {dest, src.size()};
        };

        try {
            Duration total_duration{0};
            using ArtnetReceiver = arx::artnet::Receiver<PcapStream>;
            ArtnetReceiver artnet;
            artnet.open(filename);
            RGBData dmx_frame;
            artnet.subscribe([&dmx_frame](const uint32_t universe, const uint8_t* data, const uint16_t size) -> void {
                // std::cout << "UNIVERSE " << universe << std::dec << std::endl;
                // std::cout << "size: " << std::dec << size << std::endl;
                int num_leds = size / 3;
                for (int i = 0; i < num_leds; ++ i) {
                    int j = i * 3;
                    // std::cout << "[";
                    // std::cout << static_cast<int>(data[j]) << " ";
                    // std::cout << static_cast<int>(data[j + 1]) << " ";
                    // std::cout << static_cast<int>(data[j + 2]) << " ";
                    // std::cout << "] ";
                    dmx_frame[i] = CRGB(data[j], data[j + 1], data[j + 2]);
                }
                // std::cout << std::endl;
            });
            do {
                artnet.parse();
                auto duration = artnet.getStream().getDuration();
                total_duration += duration;
                // TODO
                led_cache.emplace_back(dmx_frame, duration, total_duration);
            } while (artnet.hasPackets());
        } catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
        }
        
        current_packet = led_cache.begin();
    }

    void send_cached() {
        std::cout << "SEND " << codename << std::endl;
        
        show_playing = true;
        force_stopped = false;
        do {
            std::cout << "-- start " << codename << std::endl;
            try {
                auto background_start_time = \
                    std::chrono::duration_cast<Duration>(background_start.time_since_epoch()).count();
                // std::cout << "background_start_time: " << background_start_time << " us\n";
                auto is_playing = background_start_time > 0;
                if (background && is_playing) {
                    // show was playing in the background, rewinding...
                    TimePoint background_stop = Clock::now();
                    auto duration = std::chrono::duration_cast<Duration>(background_stop - background_start);
                    auto full_sequence_duration = std::get<2>(led_cache.back());
                    auto elapsed_from_start_duration = std::get<2>(*current_packet);
                    auto remaining_duration = full_sequence_duration - elapsed_from_start_duration;
                    std::cout << "Duration: " << duration.count() << " us\n";
                    std::cout << "full_sequence_duration: " << full_sequence_duration.count() << " us\n";
                    std::cout << "elapsed_from_start_duration: " << elapsed_from_start_duration.count() << " us\n";
                    std::cout << "remaining_duration: " << remaining_duration.count() << " us\n";
                    if (duration > remaining_duration) {
                        duration -= remaining_duration;
                        std::cout << "duration (rewind to beginning): " << duration.count() << " us\n";
                        current_packet = led_cache.begin();
                        while (duration > full_sequence_duration) {
                            duration -= full_sequence_duration;
                            // std::cout << "duration (substract full cycle): " << duration.count() << " us\n";
                            // duration %= full_sequence_duration
                        }
                    }
                    
                    std::cout << std::distance(led_cache.begin(), current_packet) << " / " << led_cache.size() << std::endl;
                    for (size_t i = std::distance(led_cache.begin(), current_packet);
                            current_packet != led_cache.end();
                            ++current_packet, ++i) {
                        // std::cout << i << " / " << led_cache.size() << std::endl;
                        auto packet_duration = std::get<1>(*current_packet);
                        if (duration > packet_duration) {
                            duration -= packet_duration;
                            // std::cout << "duration (fast forward to the packet): " << duration.count() << " us\n";
                        } else {
                            std::cout << i << " / " << led_cache.size() << std::endl;
                            break;
                        }
                    }
                    background_start = TimePoint();
                } else {
                    current_packet = led_cache.begin();
                }
                do {
                    start_processing_time = Clock::now();
                    std::this_thread::sleep_for(std::get<1>(*current_packet));
                    
                    // TODO: send
                    // int n_bytes = udp.send(std::get<0>(*current_packet));

                    ++current_packet;
                } while (current_packet != led_cache.end() && show_playing);
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl;
            }
            std::cout << "-- end " << codename << std::endl;
        } while (show_playing && (background || loop));
    }

    void stop() {
        // std::cout << "STOP " << codename << std::endl;
        show_playing = false;
        background_start = Clock::now();
    }

    void force_stop() {
        if (ignore_stop) {
            return;
        }
        // std::cout << "FORCE STOP " << codename << std::endl;
        show_playing = false;
        force_stopped = true;
    }

    void set_background(bool flag) {
        background = flag;
    }

    void set_loop(bool flag) {
        loop = flag;
    }

    void set_ignore_stop(bool flag) {
        ignore_stop = flag;
    }

    std::string get_codename() const {
        return codename;
    }
    
    std::string get_filename() const {
        return filename;
    }

    bool get_background_flag() const {
        return background;
    }

    bool get_loop_flag() const {
        return loop;
    }

    bool get_ignore_stop_flag() const {
        return ignore_stop;
    }

    bool is_force_stopped() const {
        return force_stopped;
    }

private:
    std::string codename;
    std::string filename;
    std::string hostname;
    bool background;
    bool loop;
    bool ignore_stop;
    std::atomic<bool> show_playing;
    std::atomic<bool> force_stopped;
    TimePoint start_processing_time;
    TimePoint background_start;
    // <packet data, packet duration, elapsed from start>
    
    using PacketList = std::list<std::tuple<RGBData, Duration, Duration>>;
    PacketList led_cache;
    PacketList::iterator current_packet;
};
