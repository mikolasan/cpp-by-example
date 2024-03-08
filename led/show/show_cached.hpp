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
            std::cout << "-- start [" 
                << length_us / 1000000 // milliseconds
                << "] "
                << codename 
                << std::endl;
            try {
                current_packet = udp_cache.begin();
                TimePoint start_show_time = Clock::now();
                int64_t expected_wait = 0;
                do {
                    Duration time_from_start = std::get<2>(*current_packet);
                    auto expected_time = start_show_time + time_from_start;
                    // expected_wait += std::chrono::duration_cast<std::chrono::nanoseconds>(
                    //     std::get<1>(*current_packet)).count();
                    // if (expected_wait < 100 /* ns */) {
                    //     ++current_packet;
                    //     continue;
                    // }
                    // expected_wait = 0;

                    if (time_from_start.count() > 0) {
                        std::this_thread::sleep_until(expected_time);
                    }
                    TimePoint send_time = Clock::now();
                    // int64_t diff = std::chrono::duration_cast<std::chrono::microseconds>(
                    //     send_time - expected_time).count();
                    // if (diff > 100) {
                        
                        // int64_t expected_wait = std::chrono::duration_cast<std::chrono::nanoseconds>(
                        //     std::get<1>(*current_packet)).count();
                        // std::cout << "!!! Diff in send " 
                        //     << diff
                        //     << " expected wait " << expected_wait
                        //     << " ns"
                        //     << std::endl;

                    // } else {
                        if (state.get_visible_flag()) {
                            int n_bytes = do_send(std::get<0>(*current_packet));
                        }
                    // }
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
        std::tie(udp_cache, length_us) = cache_file(filename);
        current_packet = udp_cache.begin();
    }

protected:
    uint64_t length_us; // length in nanoseconds
    TimePoint start_processing_time;
    TimePoint background_start;
    PacketList udp_cache;
    PacketList::iterator current_packet;
};
