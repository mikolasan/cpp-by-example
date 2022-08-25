#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "batteries.hpp"

const int n_reels = 5;
const int n_selected_numbers = 5;
const int n_symbols_per_reel = 24;
constexpr std::array<int, n_selected_numbers> selected_numbers = { 0,1,2,3,4 };
constexpr std::array<int, n_reels> extra_symbols = { 9,0,13,0,15 };

constexpr int encoded_numbers = n_selected_numbers + 1;
constexpr int encoded_size = encoded_numbers * n_reels;
std::map<unsigned long long, int> spot_hit_match_history;
std::map<unsigned long long, int> respin_history;
// tuple: spot_hit_match, count, original respin
std::map<unsigned long long, std::tuple<unsigned long, int>> respin_win_history;

void set_symbol(std::bitset<encoded_size>& b, int s, int reel) {
    b.set(s + encoded_numbers * reel, true);
}

void reset_symbol(std::bitset<encoded_size>& b, int s, int reel) {
    b.set(s + encoded_numbers * reel, false);
}

int get_symbol(const std::bitset<encoded_size>& b, int reel) {
    const auto bb = b >> (reel * (n_selected_numbers + 1));
    const unsigned long _6_bits = 0b111111;
    unsigned long bbb = bb.to_ulong() & _6_bits;
    for (int mask = 1, pos = 0; mask < _6_bits; mask <<= 1, ++pos) {
        if (bbb & mask) {
            return pos;
        }
    }
    return -1;
}

enum class CombinationType {
    SPOT  = 0b111111000000000000,
    HIT   = 0b000000111111000000,
    MATCH = 0b000000000000111111,
};

constexpr unsigned long encode_spot_hit_match(int spot, int hit, int match) {
    return ((1 << spot) << 12) | ((1 << hit) << 6) | (1 << match);
}

int get_number(unsigned long b, CombinationType type) {
    unsigned long bb = 0;
    switch (type) {
        case CombinationType::SPOT:
            bb = b >> 12;
            break;
        case CombinationType::HIT:
            bb = b >> 6;
            break;
        default:
            bb = b;
            break;
    }
    for (int mask = 1, pos = 0; mask < 0b111111; mask <<= 1, ++pos) {
        if (bb & mask) {
            return pos;
        }
    }
    return -1;
}

std::tuple<int, int, std::vector<int>> count(std::bitset<encoded_size>& b) {
    int s1 = get_symbol(b, 0);
    int s2 = get_symbol(b, 1);
    int s3 = get_symbol(b, 2);
    int s4 = get_symbol(b, 3);
    int s5 = get_symbol(b, 4);
    bool hit1 = std::ranges::any_of(selected_numbers, [s1](int n){ return n == s1; });
    bool hit2 = std::ranges::any_of(selected_numbers, [s2](int n){ return n == s2; });
    bool hit3 = std::ranges::any_of(selected_numbers, [s3](int n){ return n == s3; });
    bool hit4 = std::ranges::any_of(selected_numbers, [s4](int n){ return n == s4; });
    bool hit5 = std::ranges::any_of(selected_numbers, [s5](int n){ return n == s5; });
    int spots = (s1 == selected_numbers[0])
        + (s2 == selected_numbers[1])
        + (s3 == selected_numbers[2])
        + (s4 == selected_numbers[3])
        + (s5 == selected_numbers[4]);
    int hits = hit1 + hit2 + hit3 + hit4 + hit5;
    
    std::map<int, int> matches_tmp;
    ++matches_tmp[s1];
    ++matches_tmp[s2];
    ++matches_tmp[s3];
    ++matches_tmp[s4];
    ++matches_tmp[s5];
    
    int match_s1 = matches_tmp[selected_numbers[0]];
    int match_s2 = matches_tmp[selected_numbers[1]];
    int match_s3 = matches_tmp[selected_numbers[2]];
    int match_s4 = matches_tmp[selected_numbers[3]];
    int match_s5 = matches_tmp[selected_numbers[4]];
    
    std::vector<int> matches;
    matches.reserve(5);
    if (match_s1 > 0) matches.push_back(match_s1);
    if (match_s2 > 0) matches.push_back(match_s2);
    if (match_s3 > 0) matches.push_back(match_s3);
    if (match_s4 > 0) matches.push_back(match_s4);
    if (match_s5 > 0) matches.push_back(match_s5);
    return {spots, hits, matches};
}

int main(int argc, char const *argv[])
{
    std::array<int, n_selected_numbers + 1> hits;
    hits.fill(0);
    std::array<bool, n_selected_numbers> hit_per_symbol;

    std::array<int, n_selected_numbers + 1> matches;
    matches.fill(0);
    std::array<int, n_symbols_per_reel> symbols;
    symbols.fill(0);
    std::map<int, int> matches_tmp;

    std::array<int, n_selected_numbers + 1> spots;
    spots.fill(0);
    int spot_count = 0;
    int n_respins = 0;
    int n_respin_wins = 0;
    int strange = 0;
    int cycle = 0;
    int hit_freq = 0;

    std::bitset<encoded_size> current(0);

    for (int i1 = 0; i1 < n_symbols_per_reel + extra_symbols[0]; ++i1) {
        bool hit1 = std::ranges::any_of(selected_numbers, [i1](int n){ return n == i1; });
        hit_per_symbol[0] = hit1;
        // symbols[i1] += hit1 ? 1 : 0;
        spot_count = spot_count + (i1 == selected_numbers[0] ? 1 : 0);
        int sym1 = hit_per_symbol[0] ? i1 : n_selected_numbers;
        set_symbol(current, sym1, 0);
        for (int i2 = 0; i2 < n_symbols_per_reel + extra_symbols[1]; ++i2) {
            bool hit2 = std::ranges::any_of(selected_numbers, [i2](int n){ return n == i2; });
            hit_per_symbol[1] = hit2;
            // symbols[i2] += hit2 ? 1 : 0;
            spot_count = spot_count + (i2 == selected_numbers[1] ? 1 : 0);
            int sym2 = hit_per_symbol[1] ? i2 : n_selected_numbers;
            set_symbol(current, sym2, 1);
            for (int i3 = 0; i3 < n_symbols_per_reel + extra_symbols[2]; ++i3) {
                bool hit3 = std::ranges::any_of(selected_numbers, [i3](int n){ return n == i3; });
                hit_per_symbol[2] = hit3;
                // symbols[i3] += hit3 ? 1 : 0;
                spot_count = spot_count + (i3 == selected_numbers[2] ? 1 : 0);
                int sym3 = hit_per_symbol[2] ? i3 : n_selected_numbers;
                set_symbol(current, sym3, 2);
                for (int i4 = 0; i4 < n_symbols_per_reel + extra_symbols[3]; ++i4) {
                    bool hit4 = std::ranges::any_of(selected_numbers, [i4](int n){ return n == i4; });
                    hit_per_symbol[3] = hit4;
                    // symbols[i4] += hit4 ? 1 : 0;
                    spot_count = spot_count + (i4 == selected_numbers[3] ? 1 : 0);
                    int sym4 = hit_per_symbol[3] ? i4 : n_selected_numbers;
                    set_symbol(current, sym4, 3);
                    for (int i5 = 0; i5 < n_symbols_per_reel + extra_symbols[4]; ++i5) {
                        bool hit5 = std::ranges::any_of(selected_numbers, [i5](int n){ return n == i5; });
                        hit_per_symbol[4] = hit5;
                        // symbols[i5] += hit5 ? 1 : 0;
                        spot_count = spot_count + (i5 == selected_numbers[4] ? 1 : 0);
                        int sym5 = hit_per_symbol[4] ? i5 : n_selected_numbers;
                        set_symbol(current, sym5, 4);

                        int hits_count = std::ranges::count_if(hit_per_symbol, [](bool h){ return h; });
                        ++hits[hits_count];
                        
                        matches_tmp.clear();
                        ++matches_tmp[i1];
                        ++matches_tmp[i2];
                        ++matches_tmp[i3];
                        ++matches_tmp[i4];
                        ++matches_tmp[i5];
                        int match_s1 = matches_tmp[selected_numbers[0]];
                        int match_s2 = matches_tmp[selected_numbers[1]];
                        int match_s3 = matches_tmp[selected_numbers[2]];
                        int match_s4 = matches_tmp[selected_numbers[3]];
                        int match_s5 = matches_tmp[selected_numbers[4]];
                        ++matches[match_s1];
                        ++matches[match_s2];
                        ++matches[match_s3];
                        ++matches[match_s4];
                        ++matches[match_s5];
                        // symbols[i5] += hit5 ? -1 : 0;
                        
                        ++spots[spot_count];

                        int match_count = max(match_s1, match_s2, match_s3, match_s4, match_s5);
                        match_count = match_count > 1 ? match_count : 0;
                        if (hits_count <= 3 && match_count <= 3 && spot_count <= 3) {
                            if (hits_count == 3 || match_count == 3 || spot_count == 3) {
                                ++n_respins;
                                ++respin_history[current.to_ullong()];
                                unsigned long pos = encode_spot_hit_match(spot_count, hits_count, match_count);
                                ++spot_hit_match_history[pos];
                            }
                        } else {
                            if (hits_count > 3 || match_count > 3 || spot_count > 3) {
                                ++n_respin_wins;
                                unsigned long pos = encode_spot_hit_match(spot_count, hits_count, match_count);
                                auto& [spot_hit_match, count] = respin_win_history[current.to_ullong()];
                                spot_hit_match = pos;
                                ++count;
                            } else {
                                ++strange;
                            }
                        }
                        if (hits_count > 1 ||  match_count > 1 || spot_count > 0) {
                            ++hit_freq;
                        }

                        spot_count = spot_count + (i5 == selected_numbers[4] ? -1 : 0);
                        ++cycle;

                        reset_symbol(current, sym5, 4);
                    }
                    // symbols[i4] += hit4 ? -1 : 0;
                    spot_count = spot_count + (i4 == selected_numbers[3] ? -1 : 0);
                    reset_symbol(current, sym4, 3);
                }
                // symbols[i3] += hit3 ? -1 : 0;
                spot_count = spot_count + (i3 == selected_numbers[2] ? -1 : 0);
                reset_symbol(current, sym3, 2);
            }
            // symbols[i2] += hit2 ? -1 : 0;
            spot_count = spot_count + (i2 == selected_numbers[1] ? -1 : 0);
            reset_symbol(current, sym2, 1);
        }
        // symbols[i1] += hit1 ? -1 : 0;
        spot_count = spot_count + (i1 == selected_numbers[0] ? -1 : 0);
        reset_symbol(current, sym1, 0);
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
    std::cout << "RESPIN WINS : " << n_respin_wins << std::endl;
    std::cout << "STRANGE : " << strange << std::endl;

    for (const auto& [combo, n] : spot_hit_match_history) {
        std::bitset<encoded_size> b(combo);
        std::cout << b.to_string() << " ";
        std::cout << combo << " ";
        std::cout << "[";
        std::cout << get_number(combo, CombinationType::SPOT);
        std::cout << get_number(combo, CombinationType::HIT);
        std::cout << get_number(combo, CombinationType::MATCH);
        std::cout << "]: " << n << std::endl;
    }

    for (const auto& [stops, record] : respin_win_history) {
        std::bitset<encoded_size> b(stops);
        std::cout << b.to_string() << " ";
        std::cout << stops << " ";
        std::cout << "[";
        for (int r = 0; r < n_reels; ++r) {
            std::cout << (get_symbol(b, r) + 1);
        }
        auto& spot_hit_match = std::get<0>(record);
        std::cout << "]: ";
        auto [spots, hits, matches] = count(b);
        std::cout << get_number(spot_hit_match, CombinationType::SPOT);
        std::cout << "(" << spots << ")";
        std::cout << get_number(spot_hit_match, CombinationType::HIT);
        std::cout << "(" << hits << ")";
        std::cout << get_number(spot_hit_match, CombinationType::MATCH);
        std::cout << "(";
        for (auto m : matches) {
            std::cout << m << ",";
        }
        std::cout << ")";
        std::cout << " " << std::get<1>(record) << std::endl;
    }

    // for (const auto& [stops, n] : respin_history) {
    //     std::bitset<encoded_size> b(stops);
    //     std::cout << b.to_string() << " ";
    //     std::cout << stops << " ";
    //     std::cout << "[";
    //     for (int r = 0; r < n_reels; ++r) {
    //         std::cout << (get_symbol(b, r) + 1);
    //     }
    //     std::cout << "]: " << n << std::endl;
    // }

    return 0;
}
