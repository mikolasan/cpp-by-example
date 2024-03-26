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
        TimePoint start_show_time = Clock::now();
        TimePoint total_play_time = start_show_time;
        
        do {
            uint64_t length_usec = std::chrono::duration_cast<std::chrono::milliseconds>(length).count();
            std::cout << "-- start (length "  << length_usec << ") " << codename  << std::endl;

            current_packet = udp_cache.begin();
            try {
                do {
                    Duration time_from_start = std::get<2>(*current_packet);
                    auto expected_time = start_show_time + time_from_start;
                    if (time_from_start.count() > 0) {
                        std::this_thread::sleep_until(expected_time);
                    }
                    
                    if (state.get_visible_flag()) {
                        int n_bytes = do_send(std::get<0>(*current_packet));
                    }

                    if (rewind_set) {
                        state.play();
                        current_packet = udp_cache.begin();
                        start_show_time = Clock::now();
                        rewind_set = false;
                    } else {
                        ++current_packet;
                    }
                } while (current_packet != udp_cache.end() && state.is_playing());

            } catch (const std::exception& e) {
                std::cerr << e.what() << '\n';
            }
            
            std::cout << "-- end " << codename << std::endl;
            start_show_time = start_show_time + length;

        } while (state.is_playing() && state.needs_to_play());
    }

    void rewind() {
        std::cout << "-- rewind " << codename << std::endl;
        rewind_set = true;
    }
private:
    void cache(const std::string& filename) {
        std::tie(udp_cache, length) = cache_file(filename);
        current_packet = udp_cache.begin();
    }

protected:
    Duration length;
    TimePoint start_processing_time;
    TimePoint background_start;
    PacketList udp_cache;
    PacketList::iterator current_packet;
    std::atomic_bool rewind_set;
};
