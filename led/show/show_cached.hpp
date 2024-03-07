#pragma once

#include <atomic>
#include <iostream>
#include <iterator> // distance
#include <list>
#include <tuple>

#include "pcap_stream.hpp"
#include "show_abstract.hpp"


class ShowCached : public AbstractShow {
public:
    ShowCached(
        const std::string& codename,
        const std::string& filename
    ) :
        AbstractShow(codename, filename)
    {
        cache(filename);
    }

    void stop() override {
        // std::cout << "STOP " << codename << std::endl;
        state.stop();
        background_start = Clock::now();
    }

    void play() override {
        state.play();
        do {
            std::cout << "-- start " << codename << std::endl;
            try {
                auto background_start_time = std::chrono::duration_cast<Duration>(background_start.time_since_epoch()).count();
                // std::cout << "background_start_time: " << background_start_time << " us\n";
                auto is_playing = background_start_time > 0;
                if (state.get_background_flag() && is_playing) {
                    // show was playing in the background, rewinding...
                    TimePoint background_stop = Clock::now();
                    auto duration = std::chrono::duration_cast<Duration>(background_stop - background_start);
                    auto full_sequence_duration = std::get<2>(udp_cache.back());
                    auto elapsed_from_start_duration = std::get<2>(*current_packet);
                    auto remaining_duration = full_sequence_duration - elapsed_from_start_duration;
                    std::cout << "Duration: " << duration.count() << " us\n";
                    std::cout << "full_sequence_duration: " << full_sequence_duration.count() << " us\n";
                    std::cout << "elapsed_from_start_duration: " << elapsed_from_start_duration.count() << " us\n";
                    std::cout << "remaining_duration: " << remaining_duration.count() << " us\n";
                    if (duration > remaining_duration) {
                        duration -= remaining_duration;
                        std::cout << "duration (rewind to beginning): " << duration.count() << " us\n";
                        current_packet = udp_cache.begin();
                        while (duration > full_sequence_duration) {
                            duration -= full_sequence_duration;
                            // std::cout << "duration (substract full cycle): " << duration.count() << " us\n";
                            // duration %= full_sequence_duration
                        }
                    }
                    
                    std::cout << std::distance(udp_cache.begin(), current_packet) << " / " << udp_cache.size() << std::endl;
                    for (size_t i = std::distance(udp_cache.begin(), current_packet);
                            current_packet != udp_cache.end();
                            ++current_packet, ++i) {
                        // std::cout << i << " / " << udp_cache.size() << std::endl;
                        auto packet_duration = std::get<1>(*current_packet);
                        if (duration > packet_duration) {
                            duration -= packet_duration;
                            // std::cout << "duration (fast forward to the packet): " << duration.count() << " us\n";
                        } else {
                            std::cout << i << " / " << udp_cache.size() << std::endl;
                            break;
                        }
                    }
                    background_start = TimePoint();
                } else {
                    current_packet = udp_cache.begin();
                }
                do {
                    start_processing_time = Clock::now();
                    std::this_thread::sleep_for(std::get<1>(*current_packet));
                    // int n_bytes = udp.send(std::get<0>(*current_packet));
                    int n_bytes = do_send(std::get<0>(*current_packet));
                    ++current_packet;
                } while (current_packet != udp_cache.end() && state.is_playing());
            } catch (const std::exception& e) {
                std::cerr << e.what() << '\n';
            }
            std::cout << "-- end " << codename << std::endl;
        } while (state.is_playing() && state.needs_to_play());
    }

private:
    void cache(const std::string& filename) {
        udp_cache = cache_file(filename);
        current_packet = udp_cache.begin();
    }

protected:
    TimePoint start_processing_time;
    TimePoint background_start;
    PacketList udp_cache;
    PacketList::iterator current_packet;
};
