#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

#include "Artnet.h"
#include "pcap_stream.hpp"
#include "stb_vorbis.h"

struct show_info {
    std::list<size_t> packets;
    size_t prev_show;
    std::list<bool> all_zero;
};

std::chrono::microseconds read_ogg_audio_duration(const std::string& ogg_file) {
    std::ifstream in(ogg_file, std::ios::in | std::ios::binary);
    if (!in) {
        std::cerr << "could not open " << ogg_file << std::endl;
        throw std::runtime_error("Failed to open OGG file");
    }
    std::ostringstream contents;
    contents << in.rdbuf();
    std::string ogg_data = contents.str();

    int error;
    stb_vorbis* vorbis = stb_vorbis_open_memory(
        reinterpret_cast<const unsigned char*>(ogg_data.data()),
        ogg_data.size(),
        &error,
        NULL);

    if (!vorbis) {
        throw std::runtime_error("Failed to open OGG file with stb_vorbis");
    }

    double total_seconds = stb_vorbis_stream_length_in_seconds(vorbis);
    std::chrono::microseconds duration(static_cast<int64_t>(total_seconds * 1000000));
    
    stb_vorbis_close(vorbis);

    return duration;
}

int main(int argc, char const *argv[])
{
    if (argc != 4) {
        std::cerr << "this program needs (input file, output file, sound file) as input arguments" << std::endl;
        return 1;
    }

    std::string ogg_file(argv[3]);
    auto audio_duration = read_ogg_audio_duration(ogg_file);
    
    std::string input(argv[1]);
    std::string output(argv[2]);
    size_t packet_id = 0;
    size_t first_show_packet = 0;
    size_t last_show_packet = 0;
    
    try {
        using ArtnetReceiver = arx::artnet::Receiver<PcapStream>;
        {
            ArtnetReceiver artnet;
            artnet.open(input);

            bool forward_chain_broken = false;
            bool backward_chain_broken = false;
            show_info prev_show;
            show_info current_show;
            artnet.subscribe([
                &packet_id,
                &prev_show,
                &current_show,
                &first_show_packet,
                &last_show_packet,
                &forward_chain_broken,
                &backward_chain_broken
            ] (const uint32_t universe, const uint8_t* data, const uint16_t size) -> void {
                bool all_zero = true;
                for (int i = 0; i < size / 3; ++ i) {
                    all_zero = all_zero && ((data[3*i] + data[3*i + 1] + data[3*i + 2]) == 0);
                }

                if (current_show.packets.empty() && universe != 0) {
                    std::cout << "packet " << packet_id << " relates to universe " << universe << std::endl;
                    return;
                } else if (!current_show.packets.empty() && universe == 0) {

                    bool prev_all_zero = std::ranges::all_of(prev_show.all_zero, [](bool b){ return b; });
                    bool current_all_zero = std::ranges::all_of(current_show.all_zero, [](bool b){ return b; });
                    if (!forward_chain_broken && current_all_zero && prev_all_zero) {
                        first_show_packet = packet_id;
                    } else {
                        forward_chain_broken = true;
                    }

                    if (!(forward_chain_broken && current_all_zero && prev_all_zero)) {
                        last_show_packet = prev_show.packets.back();
                    }

                    prev_show = current_show;
                    current_show.prev_show = prev_show.packets.back();

                    current_show.packets.clear();
                    current_show.packets.push_back(packet_id);

                    current_show.all_zero.clear();
                    current_show.all_zero.push_back(all_zero);
                } else {
                    current_show.packets.push_back(packet_id);

                    current_show.all_zero.push_back(all_zero);
                }
            });
            std::cout << "Processing '" << input << "'..." << std::endl;
            while (artnet.hasPackets()) {
                artnet.parse();
                ++packet_id;
            }
            std::cout << "First show packet " << first_show_packet << std::endl;
            std::cout << "Last show packet " << last_show_packet << std::endl;
        }
        {
            packet_id = 0;
            ArtnetReceiver artnet;
            artnet.open(input);
            auto& stream = artnet.getStream();
            stream.output(output);
            Duration total_duration{0};
            Duration total_show_duration{0};
            Duration total_out_duration{0};
            Duration duration_between_packets{0};
            size_t out_packet_id = 0;
            
            std::cout << "Writing to '" << output << "'..." << std::endl;
            while (artnet.hasPackets()) {
                auto code = artnet.parse();
                auto duration = stream.getDuration();
                total_duration += duration;
                
                if (first_show_packet <= packet_id && packet_id <= last_show_packet) {
                    if (out_packet_id > 0) {
                        total_show_duration += duration; // skip the first duration
                        duration_between_packets += duration;
                    }
                    if (code == arx::artnet::OpCode::Dmx) { //  || code == arx::artnet::OpCode::Sync

                        if (total_out_duration < audio_duration) {
                            if (total_out_duration + duration_between_packets > audio_duration) {
                                // update duration only for the last packet ignoring any packets that we skippd
                                // the pcap library will handle that during the dump
                                Duration new_duration_between_packets = audio_duration - total_out_duration;
                                Duration diff = duration_between_packets - new_duration_between_packets;
                                // std::cout << "final length " << std::chrono::duration_cast<std::chrono::microseconds>(total_out_duration).count() << std::endl;
                                // std::cout << "new last frame duration: " << duration_between_packets.count() << std::endl;
                                stream.setDuration(duration - diff);
                                duration_between_packets = new_duration_between_packets;
                            }

                            total_out_duration += duration_between_packets;
                            // std::cout << out_packet_id << " "
                            //     << duration_between_packets.count() << " "
                            //     << total_out_duration.count() << std::endl;
                        
                            stream.dumpPacket();
                            
                            ++out_packet_id;
                            duration_between_packets = std::chrono::nanoseconds(0);
                        } else {
                            // std::cout << "{{beyond audio}} "
                            //     << duration.count() << " "
                            //     << total_show_duration.count() << std::endl;
                        }
                    } else {
                        // std::cout << "<<skip packet>> "
                        //     << duration.count() << " "
                        //     << total_show_duration.count() << std::endl;
                    }
                }
                ++packet_id;
            }
            std::cout << "Audio duration: " 
                << audio_duration.count()
                << " usec" << std::endl;
            std::cout << "Output show duration: " 
                << std::chrono::duration_cast<std::chrono::microseconds>(total_out_duration).count()
                << " usec" << std::endl;
            std::cout << "Captured show duration: " 
                << std::chrono::duration_cast<std::chrono::microseconds>(total_show_duration).count()
                << " usec" << std::endl;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    

    std::cout << "Done" << std::endl;

    return 0;
}
