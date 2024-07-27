#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <random>
#include <string>
#include <vector>

using BagOfWords = std::vector<std::string>;
using WordTable = std::vector<std::vector<std::string>>;

BagOfWords sort_words_by_length(const BagOfWords& w) {
	BagOfWords x(w);
	std::sort(
		std::begin(x),
		std::end(x),
		[](const std::string& a, const std::string& b) {
			return a.length() < b.length();
		});
	return x;
}

std::multimap<int, std::string> map_words_by_length(const BagOfWords& w) {
	std::multimap<int, std::string> m;
	std::transform(
		std::begin(w),
		std::end(w),
		std::inserter(m, m.begin()),
		[](const std::string& s) { 
			return std::make_pair(s.length(), s);
		});
	return m;
}

void print_word_table(const WordTable& t) {
	for (int i = 0; i < t.size(); ++i) {
		for (int j = 0; j < t[i].size(); ++j) {
			std::cout << t[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

WordTable combine_1(const BagOfWords& words, int side) {
	WordTable clock(side, BagOfWords(side, "_"));
	auto sorted = sort_words_by_length(words);
	auto it = sorted.begin();
	for (int i = 0; i < side; ++i) {
		for (int j = 0; j < side; ++j) {
			if (it->length() < side - j) {
				auto c = clock[i].begin();
				std::advance(c, j);
				std::copy(it->begin(), it->end(), c);
				j += it->length() - 1;
				++it;
			}
		}
	}
	return clock;
}

WordTable combine_2(const BagOfWords& words, int side) {
	WordTable clock(side, BagOfWords(side, "_"));
	auto m = map_words_by_length(words);
	return clock;
}

int main() {
	const int side = 9;
	BagOfWords words = {
	  "quarter", "half", "past", "to",

	  "twenty", "ten", "five", "twelve",
	  "one", "two", "three", "four", "five",
	  "six", "seven", "eight", "nine", "ten",
	  "eleven"
	};

	auto clock = combine_1(words, side);
	print_word_table(clock);

	auto m = map_words_by_length(words);
	for (auto&& [k, v] : m) {
		std::cout << k << " " << v << std::endl;
	}
	//   auto groups = groups_in_length(words, side);
	//   std::random_device device;
	//   std::mt19937 generator(device());
	//   std::shuffle(std::begin(groups), std::end(groups), generator);
	//   auto it = groups.begin();
	//   BagOfWords clock2;
	//   for (int i = 0; i < side; ++i) {
	//       std::string s = std::accumulate(it->begin(), it->end(), std::string());
	//       clock2.push_back(s);
	//   }

	//   for (int i = 0; i < side; ++i) {
	//     for (int j = 0; j < side; ++j) {
	//       std::cout << clock2[i][j] << " ";
	//     }
	//     std::cout << std::endl;
	//   }

	return 0;
}