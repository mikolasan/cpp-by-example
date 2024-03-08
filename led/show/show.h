#pragma once

#include <chrono>
#include <list>
#include <string>
#include <string_view>
#include <tuple>

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

using Duration = std::chrono::nanoseconds;
using Data = std::string_view;
// <packet data, packet duration, elapsed from start>
using PacketList = std::list<std::tuple<Data, Duration, Duration>>;

std::string get_file_contents(const char *filename);
std::tuple<PacketList, uint64_t> cache_file(const std::string& filename);