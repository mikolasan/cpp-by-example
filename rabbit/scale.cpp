#include <iostream>
#include <limits>
#include <random>

constexpr auto max_size = std::numeric_limits<std::streamsize>::max();

int main(int argc, char const *argv[])
{
    uint64_t range_upper_limit = std::atoll(argv[1]);
    int64_t input_number = 0;

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

            // Classic Modulo (Biased)
            uint32_t random_number = static_cast<uint32_t>(input_number % range_upper_limit);

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
            // uint32_t random_number = static_cast<uint32_t>(input_number);
            // uint32_t range = static_cast<uint32_t>(range_upper_limit);
            // uint32_t t = (-range) % range;
            // if (random_number >= t)
            //     random_number = random_number % range;
            // else
            //     continue; // bad random number, use another one

            std::cout << random_number << std::endl;
        }
    } while (true);

    return 0;
}