#include <algorithm>
#include <array>
#include <iostream>

#include "batteries.hpp"

const int n_selected_numbers = 5;
const int n_symbols_per_reel = 24;
constexpr std::array<int, n_selected_numbers> selected_numbers = { 1,2,3,4,5 };

int main(int argc, char const *argv[])
{
    std::array<int, n_selected_numbers + 1> hits;
    hits.fill(0);

    std::array<bool, n_selected_numbers> hit_per_symbol;
    for (int i1 = 0; i1 < n_symbols_per_reel; ++i1) {
        hit_per_symbol[0] = std::ranges::any_of(selected_numbers, [i1](int n){ return n == i1; });
        for (int i2 = 0; i2 < n_symbols_per_reel; ++i2) {
            hit_per_symbol[1] = std::ranges::any_of(selected_numbers, [i2](int n){ return n == i2; });
            for (int i3 = 0; i3 < n_symbols_per_reel; ++i3) {
                hit_per_symbol[2] = std::ranges::any_of(selected_numbers, [i3](int n){ return n == i3; });
                for (int i4 = 0; i4 < n_symbols_per_reel; ++i4) {
                    hit_per_symbol[3] = std::ranges::any_of(selected_numbers, [i4](int n){ return n == i4; });
                    for (int i5 = 0; i5 < n_symbols_per_reel; ++i5) {
                        hit_per_symbol[4] = std::ranges::any_of(selected_numbers, [i5](int n){ return n == i5; });
                        int count = std::ranges::count_if(hit_per_symbol, [](bool h){ return h; });
                        ++hits[count];
                    }
                }
            }
        }
    }

    std::cout << std::endl;
    std::cout << "HITS" << std::endl;
    for (const auto& [i, hit] :enumerate(hits)) {
        std::cout << "x" << i << " " << hit << std::endl;
    }

    /////////////////////////////////////////////

    std::array<int, n_selected_numbers + 1> matches;
    matches.fill(0);

    std::array<int, n_symbols_per_reel> symbols;
    symbols.fill(0);
    for (int i1 = 0; i1 < n_symbols_per_reel; ++i1) {
        bool match1 = std::ranges::any_of(selected_numbers, [i1](int n){ return n == i1; });
        symbols[i1] += match1 ? 1 : 0;
        for (int i2 = 0; i2 < n_symbols_per_reel; ++i2) {
            bool match2 = std::ranges::any_of(selected_numbers, [i2](int n){ return n == i2; });
            symbols[i2] += match2 ? 1 : 0;
            for (int i3 = 0; i3 < n_symbols_per_reel; ++i3) {
                bool match3 = std::ranges::any_of(selected_numbers, [i3](int n){ return n == i3; });
                symbols[i3] += match3 ? 1 : 0;
                for (int i4 = 0; i4 < n_symbols_per_reel; ++i4) {
                    bool match4 = std::ranges::any_of(selected_numbers, [i4](int n){ return n == i4; });
                    symbols[i4] += match4 ? 1 : 0;
                    for (int i5 = 0; i5 < n_symbols_per_reel; ++i5) {
                        bool match5 = std::ranges::any_of(selected_numbers, [i5](int n){ return n == i5; });
                        symbols[i5] += match5 ? 1 : 0;
                        ++matches[symbols[selected_numbers[0]]];
                        ++matches[symbols[selected_numbers[1]]];
                        ++matches[symbols[selected_numbers[2]]];
                        ++matches[symbols[selected_numbers[3]]];
                        ++matches[symbols[selected_numbers[4]]];
                        symbols[i5] += match5 ? -1 : 0;
                    }
                    symbols[i4] += match4 ? -1 : 0;
                }
                symbols[i3] += match3 ? -1 : 0;
            }
            symbols[i2] += match2 ? -1 : 0;
        }
        symbols[i1] += match1 ? -1 : 0;
    }

    std::cout << std::endl;
    std::cout << "MATCHES" << std::endl;
    for (const auto& [i, match] :enumerate(matches)) {
        std::cout << "x" << i << " " << match << std::endl;
    }


    /////////////////////////////////////////////

    std::array<int, n_selected_numbers + 1> spots;
    spots.fill(0);

    int count = 0;
    for (int i1 = 0; i1 < n_symbols_per_reel; ++i1) {
        count = count + (i1 == selected_numbers[0] ? 1 : 0);
        for (int i2 = 0; i2 < n_symbols_per_reel; ++i2) {
            count = count + (i2 == selected_numbers[1] ? 1 : 0);
            for (int i3 = 0; i3 < n_symbols_per_reel; ++i3) {
                count = count + (i3 == selected_numbers[2] ? 1 : 0);
                for (int i4 = 0; i4 < n_symbols_per_reel; ++i4) {
                    count = count + (i4 == selected_numbers[3] ? 1 : 0);
                    for (int i5 = 0; i5 < n_symbols_per_reel; ++i5) {
                        count = count + (i5 == selected_numbers[4] ? 1 : 0);
                        ++spots[count];
                        count = count + (i5 == selected_numbers[4] ? -1 : 0);
                    }
                    count = count + (i4 == selected_numbers[3] ? -1 : 0);
                }
                count = count + (i3 == selected_numbers[2] ? -1 : 0);
            }
            count = count + (i2 == selected_numbers[1] ? -1 : 0);
        }
        count = count + (i1 == selected_numbers[0] ? -1 : 0);
    }

    std::cout << std::endl;
    std::cout << "SPOTS" << std::endl;
    for (const auto& [i, spot] :enumerate(spots)) {
        std::cout << "x" << i << " " << spot << std::endl;
    }

    return 0;
}
