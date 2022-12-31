#include <cstdint>
#include <iostream>
#include <limits>

constexpr auto max_size = std::numeric_limits<std::streamsize>::max();

int main(int argc, char const *argv[])
{
    int64_t input_number = 0;
    while (true) {
        std::cin >> input_number;
        if (std::cin.eof() || std::cin.bad()) {
            break;
        } else if (std::cin.fail()) {
            std::cin.clear(); // unset failbit
            std::cin.ignore(max_size, '\n'); // skip bad input
        } else {
            if (input_number < 0)
                input_number += 0x100000000LL;
            char c = (input_number & 0xFF000000) >> 24;
            std::cout << c;
            c = (input_number & 0x00FF0000) >> 16;
            std::cout << c;
            c = (input_number & 0x0000FF00) >> 8;
            std::cout << c;
            c = (input_number & 0x000000FF);
            std::cout << c;
        }
    }
    return 0;
}
