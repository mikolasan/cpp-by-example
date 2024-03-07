#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "pcap_stream.hpp"
#include "show_abstract.h"

std::string get_file_contents(const char *filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in) {
        std::cerr << "could not open " << filename << std::endl;
        exit(1);
    }
    std::ostringstream contents;
    contents << in.rdbuf();
    return contents.str();
}

const auto copy = [](std::string_view src) -> std::string_view {
    using Char = std::string_view::value_type;
    Char* const dest = new Char[src.size()];
    src.copy(dest, src.size());
    return {dest, src.size()};
};

PacketList cache_file(const std::string& filename) {
    PacketList udp_cache;
    PcapStream stream{filename};
    try {
        Duration total_duration{0};
        do {
            const size_t size = stream.parsePacket();
            if (size == 0) continue;
            auto duration = stream.getDuration();
            total_duration += duration;
            udp_cache.emplace_back(copy(stream.getData()), duration, total_duration);
        } while (stream.hasPackets());
    } catch (...) {
        std::cerr << "caching error :(" << std::endl;
    }
    return udp_cache;
}
