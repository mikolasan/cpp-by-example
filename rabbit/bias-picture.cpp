#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <random>
#include <vector>

constexpr auto max_size = std::numeric_limits<std::streamsize>::max();
constexpr auto max_uint = std::numeric_limits<std::uint32_t>::max();

int main(int argc, char const *argv[])
{
    const int size = 50;
    std::array<std::array<uint64_t, size>, size> picture;
    picture.fill(std::array<uint64_t, size>());

    std::optional<uint32_t> x, y;
    
    std::map<uint32_t, uint64_t> m;
    // std::vector<double> observed;

    uint64_t n_processed = 0;
    uint64_t total_sum = 0;
    uint64_t range_upper_limit = std::atoll(argv[1]);
    uint64_t sample_size = std::atoll(argv[2]);
    // observed.reserve(sample_size);
    int64_t input_number = 0;
    
    srand(0);

    do {
        std::cin >> input_number;
        if (std::cin.eof() || std::cin.bad()) {
            break;
        } else if (std::cin.fail()) {
            std::cin.clear(); // unset failbit
            std::cin.ignore(max_size, '\n'); // skip bad input
        } else {
            if (input_number < 0)
                input_number += 0x100000000LL;

            // input_number = rand();
            
            // Classic Modulo (Biased)
            // uint32_t random_number = static_cast<uint32_t>(input_number % range_upper_limit);


            // Division with Rejection (Unbiased)
            // uint32_t random_number = 0;
            // uint32_t range = static_cast<uint32_t>(range_upper_limit);
            // // calculates divisor = 2**32 / range
            // uint32_t divisor = ((-range) / range) + 1;
            // if (divisor != 0) {
            //     random_number = static_cast<uint32_t>(input_number) / divisor;
            //     if (random_number >= range) {
            //         continue; // bad random number, use another one
            //     }
            // }

            // Debiased Modulo (Twice) — OpenBSD's Method
            uint32_t random_number = static_cast<uint32_t>(input_number);
            uint32_t range = static_cast<uint32_t>(range_upper_limit);
            // uint32_t t = (max_uint - range) % range;
            uint32_t t = max_uint - (max_uint % range);
            if (random_number >= t)
                continue; // bad random number, use another one
            else
                random_number = random_number % range;

            std::cout << random_number << std::endl;
            ++n_processed;
            // m[random_number]++;

            // if (x.has_value()) {
            //     y = random_number;
            //     ++picture[x.value()][y.value()];
            //     x.reset();
            //     y.reset();
            // } else {
            //     x = random_number;
            // }
            
            // total_sum += input_number;
        }
        
        
        // // std::uniform_int_distribution<uint32_t> dist(0, range_upper_limit - 1);
        // // auto random_number = dist(rng);

        // // uint32_t random_number = rand() % range_upper_limit;

        // m[random_number]++;
        // ++n_processed;

        // if (n_processed > 0 && (n_processed + 1) % sample_size == 0) {
        //     for (size_t n = 0; n < m.size(); ++n) {
        //         std::cout << m.at(n) << ',';
        //     }
        //     std::cout << '\n';
        //     m.clear();
        //     // srand(0);
        // }

        if (n_processed == sample_size) {
            // double expected = sample_size / range_upper_limit;
            // // std::cout << "expected " << expected << std::endl;
            // // for (const auto& [number, hits] : m) {
            // //     std::cout << number << ": " << hits << std::endl;
            // // }
            // chi_squared_test(m, expected);
            // n_processed = 0;
            // m.clear();
            break;
            // std::cout << "----------------------------------------------" << std::endl;
        }

    } while (true);
    
    // for (size_t y = 0; y < size; ++y) {
    //     for (size_t x = 0; x < size; ++x) {
    //         std::cout << picture[x][y] << ',';
    //     }
    //     std::cout << '\n';
    // }
    
    // for (const auto& [number, hits] : m) {
    //     std::cout << number << ": " << hits / static_cast<double>(n_processed) << " [" << hits << "]" << std::endl;
    // }
    
    // double expected = sample_size / range_upper_limit;
    // chi_squared_test(m, expected);

    // std::cout << "AVG: " << total_sum / static_cast<double>(range_upper_limit) << std::endl;

    return 0;
}