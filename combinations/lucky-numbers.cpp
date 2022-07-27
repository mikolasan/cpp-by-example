#include <algorithm>
#include <array>
#include <iostream>

#include "batteries.hpp"

const int n_reels = 5;
const int n_selected_numbers = 5;
const int n_symbols_per_reel = 24;
constexpr std::array<int, n_selected_numbers> selected_numbers = { 1,2,3,4,5 };
constexpr std::array<int, n_reels> extra_symbols = { 5,0,7,0,8 };

int main(int argc, char const *argv[])
{
    std::array<int, n_selected_numbers + 1> hits;
    hits.fill(0);
    std::array<bool, n_selected_numbers> hit_per_symbol;

    std::array<int, n_selected_numbers + 1> matches;
    matches.fill(0);
    std::array<int, n_symbols_per_reel> symbols;
    symbols.fill(0);

    std::array<int, n_selected_numbers + 1> spots;
    spots.fill(0);
    int spot_count = 0;
    int n_respins = 0;
    int cycle = 0;
    int hit_freq = 0;

    for (int i1 = 0; i1 < n_symbols_per_reel + extra_symbols[0]; ++i1) {
        hit_per_symbol[0] = std::ranges::any_of(selected_numbers, [i1](int n){ return n == i1; });
        bool match1 = std::ranges::any_of(selected_numbers, [i1](int n){ return n == i1; });
        symbols[i1] += match1 ? 1 : 0;
        spot_count = spot_count + (i1 == selected_numbers[0] ? 1 : 0);
        for (int i2 = 0; i2 < n_symbols_per_reel + extra_symbols[1]; ++i2) {
            hit_per_symbol[1] = std::ranges::any_of(selected_numbers, [i2](int n){ return n == i2; });
            bool match2 = std::ranges::any_of(selected_numbers, [i2](int n){ return n == i2; });
            symbols[i2] += match2 ? 1 : 0;
            spot_count = spot_count + (i2 == selected_numbers[1] ? 1 : 0);
            for (int i3 = 0; i3 < n_symbols_per_reel + extra_symbols[2]; ++i3) {
                hit_per_symbol[2] = std::ranges::any_of(selected_numbers, [i3](int n){ return n == i3; });
                bool match3 = std::ranges::any_of(selected_numbers, [i3](int n){ return n == i3; });
                symbols[i3] += match3 ? 1 : 0;
                spot_count = spot_count + (i3 == selected_numbers[2] ? 1 : 0);
                for (int i4 = 0; i4 < n_symbols_per_reel + extra_symbols[3]; ++i4) {
                    hit_per_symbol[3] = std::ranges::any_of(selected_numbers, [i4](int n){ return n == i4; });
                    bool match4 = std::ranges::any_of(selected_numbers, [i4](int n){ return n == i4; });
                    symbols[i4] += match4 ? 1 : 0;
                    spot_count = spot_count + (i4 == selected_numbers[3] ? 1 : 0);
                    for (int i5 = 0; i5 < n_symbols_per_reel + extra_symbols[4]; ++i5) {
                        hit_per_symbol[4] = std::ranges::any_of(selected_numbers, [i5](int n){ return n == i5; });
                        int hits_count = std::ranges::count_if(hit_per_symbol, [](bool h){ return h; });
                        ++hits[hits_count];
                        
                        bool match5 = std::ranges::any_of(selected_numbers, [i5](int n){ return n == i5; });
                        symbols[i5] += match5 ? 1 : 0;
                        ++matches[symbols[selected_numbers[0]]];
                        ++matches[symbols[selected_numbers[1]]];
                        ++matches[symbols[selected_numbers[2]]];
                        ++matches[symbols[selected_numbers[3]]];
                        ++matches[symbols[selected_numbers[4]]];
                        symbols[i5] += match5 ? -1 : 0;
                        
                        spot_count = spot_count + (i5 == selected_numbers[4] ? 1 : 0);
                        ++spots[spot_count];

                        int match_count = symbols[selected_numbers[0]] + symbols[selected_numbers[1]] + symbols[selected_numbers[2]] + symbols[selected_numbers[3]] + symbols[selected_numbers[4]];
                        if (hits_count <= 3 && match_count <= 3 && spot_count <= 3 &&
                                (hits_count == 3 || match_count == 3 || spot_count == 3)) {
                            ++n_respins;
                        }
                        if (hits_count > 1 ||  match_count > 1 || spot_count > 0) {
                            ++hit_freq;
                        }

                        spot_count = spot_count + (i5 == selected_numbers[4] ? -1 : 0);
                        ++cycle;
                    }
                    symbols[i4] += match4 ? -1 : 0;
                    spot_count = spot_count + (i4 == selected_numbers[3] ? -1 : 0);
                }
                symbols[i3] += match3 ? -1 : 0;
                spot_count = spot_count + (i3 == selected_numbers[2] ? -1 : 0);
            }
            symbols[i2] += match2 ? -1 : 0;
            spot_count = spot_count + (i2 == selected_numbers[1] ? -1 : 0);
        }
        symbols[i1] += match1 ? -1 : 0;
        spot_count = spot_count + (i1 == selected_numbers[0] ? -1 : 0);
    }
    
    std::cout << std::endl;
    std::cout << "HITS" << std::endl;
    for (const auto& [i, hit] :enumerate(hits)) {
        std::cout << "x" << i << " " << hit << std::endl;
    }

    /////////////////////////////////////////////

    std::cout << std::endl;
    std::cout << "MATCHES" << std::endl;
    for (const auto& [i, match] :enumerate(matches)) {
        std::cout << "x" << i << " " << match << std::endl;
    }

    /////////////////////////////////////////////

    std::cout << std::endl;
    std::cout << "SPOTS" << std::endl;
    for (const auto& [i, spot] :enumerate(spots)) {
        std::cout << "x" << i << " " << spot << std::endl;
    }

    std::cout << std::endl;
    std::cout << "CYCLE   : " << cycle << std::endl;
    std::cout << "HIT FREQ: " << hit_freq << std::endl;
    std::cout << "RESPINS : " << n_respins << std::endl;

    return 0;
}
