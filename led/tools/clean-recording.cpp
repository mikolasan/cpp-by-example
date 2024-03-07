#include <algorithm>
#include <iomanip>
#include <iostream>
#include <list>
#include <string>

#include "Artnet.h"
#include "pcap_stream.h"

struct show_info {
    std::list<size_t> packets;
    size_t prev_show;
    std::list<bool> all_zero;
};

int main(int argc, char const *argv[])
{
    if (argc != 3) {
        std::cerr << "this program needs (input file, output file) as input arguments" << std::endl;
        return 1;
    }


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
                &backward_chain_broken]
            (const uint32_t universe, const uint8_t* data, const uint16_t size) -> void {
                bool all_zero = true;
                for (int i = 0; i < size / 3; ++ i) {
                    all_zero = all_zero && ((data[3*i] + data[3*i + 1] + data[3*i + 2]) == 0);
                }

                if (current_show.packets.empty() && universe != 0) {
                    std::cout << "packet " << packet_id << " reletes to universe " << universe << std::endl;
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
            artnet.getStream().output(output);
            std::cout << "Writing to '" << output << "'..." << std::endl;
            while (artnet.hasPackets()) {
                auto code = artnet.parse();
                if (first_show_packet <= packet_id
                        && packet_id <= last_show_packet
                        && code == arx::artnet::OpCode::Dmx) {
                    artnet.getStream().dumpPacket();
                }
                ++packet_id;
            }
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    std::cout << "Done" << std::endl;

    return 0;
}
