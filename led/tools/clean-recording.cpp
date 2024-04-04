#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <optional>
#include <regex>
#include <sstream>
#include <string>

#include <ryml_std.hpp>
#include <ryml.hpp>
#include <stb_vorbis.h>

#include "Artnet.h"
#include "config.hpp"
#include "pcap_stream.hpp"
#include "show.h"

using ArtnetReceiver = arx::artnet::Receiver<PcapStream>;

struct show_info {
    std::list<size_t> packets;
    size_t prev_show;
    std::list<bool> all_zero;
};

// (game_name):(sound_name)
const std::regex show_mapping_regex("([a-z_]*)\\|(.*)", std::regex_constants::ECMAScript);

std::chrono::microseconds read_ogg_audio_duration(const std::string& ogg_file) {
    std::ifstream in(ogg_file, std::ios::in | std::ios::binary);
    if (!in) {
        std::cerr << "could not open '" << ogg_file << "'\n";
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

std::pair<size_t, size_t> find_non_empty_range(const std::string& pcap_filename) {
    ArtnetReceiver artnet;
    artnet.open(pcap_filename);

    size_t first_show_packet = 0;
    size_t last_show_packet = 0;
    bool forward_chain_broken = false;
    bool backward_chain_broken = false;
    show_info prev_show;
    show_info current_show;
    size_t packet_id = 0;
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
    
    while (artnet.hasPackets()) {
        artnet.parse();
        ++packet_id;
    }
    std::cout << "First show packet " << first_show_packet << std::endl;
    std::cout << "Last show packet " << last_show_packet << std::endl;

    return {first_show_packet, last_show_packet};
}

void save_range(
    const std::string& input,
    const std::string& output,
    const std::pair<size_t, size_t>& range,
    const std::optional<std::string>& ogg_filename)
{
    auto audio_duration = std::chrono::microseconds(0);
    if (ogg_filename) {
        audio_duration = read_ogg_audio_duration(ogg_filename.value());
    }

    size_t packet_id = 0;
    auto [first_show_packet, last_show_packet] = range;
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
            if (code == arx::artnet::OpCode::Dmx || code == arx::artnet::OpCode::Sync) {

                if (ogg_filename) {
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
                    total_out_duration += duration_between_packets;
                    stream.dumpPacket();
                    
                    ++out_packet_id;
                    duration_between_packets = std::chrono::nanoseconds(0);
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

int main(int argc, char const *argv[])
{
    if (argc != 5) {
        std::cerr << "this program needs (input dir, output dir, sounds dir, config file) as input arguments" << std::endl;
        return 1;
    }

    namespace fs = std::filesystem;
    fs::path input_dir{argv[1]};
    fs::path output_dir{argv[2]};
    fs::path sounds_dir{argv[3]};

    std::string conifg_file(argv[4]);
    std::string contents = get_file_contents(conifg_file.c_str());
    ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(contents));

    ryml::NodeRef show_mapping = tree["show_mapping"];

    // cache all animations
    for (ryml::NodeRef const& child : show_mapping.children()) {
        std::string show_id;
        ryml::from_chars(child.key(), &show_id);

        // std::cout << child.has_child(ryml::to_csubstr("show_file")) << std::endl;
        std::string show_file;
        child["show_file"] >> show_file;

        std::smatch match;
        std::regex_match(show_id, match, show_mapping_regex);

        std::ssub_match game_name_sub_match = match[1];
        std::string game_name = game_name_sub_match.str();

        std::ssub_match sound_name_sub_match = match[2];
        std::string sound_name = sound_name_sub_match.str();

        if (sound_name.empty()) {
            std::cout << "skipping show command [" << show_id << "]" << std::endl;
            continue;
        }

        std::string input((input_dir / show_file).string());
        std::string output((output_dir / show_file).string());
        std::optional<std::string> ogg_filename((sounds_dir / sound_name).string());
        bool perfect_with_sound = true;
        if (child.has_child(ryml::to_csubstr("perfect_with_sound"))) {
            child["perfect_with_sound"] >> perfect_with_sound;
            if (!perfect_with_sound) {
                ogg_filename.reset();
            }
        }

        std::cout << std::endl;
        std::cout << "Processing '" << input << "'..." << std::endl;
        std::cout << std::endl;
        
        try {
            auto range = find_non_empty_range(input);
            save_range(input, output, range, ogg_filename);
        } catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }

    std::cout << std::endl;
    std::cout << "Done" << std::endl;
    return 0;
}
