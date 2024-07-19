// HackerRankContest.cpp : Defines the entry point for the application.
//

#include "HackerRankContest.h"

using namespace std;

class NewKeyboardTest : public ::testing::Test {
protected:
	string input_a = string(500000, 'A');
	string input_b = string(500000, 'B');
	string input_c = string(500000, 'C');
	string input_clear = string(500000, '*');
	string long_clear = input_a + input_clear;
	string swap_blocks = input_a + "<" + input_b;
	string swap_blocks_answer = input_b + input_a;
};

string receivedTextNaiveString(string S) {
	string out;
	auto it = out.begin();
	bool num_lock = true;
	for (auto c : S) {
		if ('<' == c) { // Home
			it = out.begin();
		}
		else if ('>' == c) { // End
			it = out.end();
		}
		else if ('*' == c) { // Backspace
			if (it != out.begin()) {
				--it; // point to character
				it = out.erase(it);
			}
		}
		else if ('#' == c) { // Num Lock
			num_lock = !num_lock;
		}
		else if (!num_lock && '0' <= c && c <= '9') {
			; // num lock is off
		}
		else {
			it = out.insert(it, c);
			++it; // next pos
		}
	}
	return out;
}

string receivedText(string S) {
	list<string::value_type> out;
	auto it = out.begin();
	bool num_lock = true;
	for (auto c : S) {
		if ('<' == c) { // Home
			it = out.begin();
		}
		else if ('>' == c) { // End
			it = out.end();
		}
		else if ('*' == c) { // Backspace
			if (it != out.begin()) {
				--it; // point to character
				it = out.erase(it);
			}
		}
		else if ('#' == c) { // Num Lock
			num_lock = !num_lock;
		}
		else if (!num_lock && '0' <= c && c <= '9') {
			; // num lock is off
		}
		else {
			it = out.insert(it, c);
			++it; // next pos
		}
	}
	return string(out.begin(), out.end());
}

string receivedTextVector(string S) {
	vector<string::value_type> out;
	out.reserve(S.size());
	auto it = out.begin();
	auto insert_begin = S.end();
	auto insert_end = S.begin();
	bool num_lock = true;
	for (auto s_it = S.begin(); s_it != S.end(); ++s_it) {
		auto c = *s_it;
		if ('<' == c) { // Home
			if (insert_begin != insert_end) {
				it = out.insert(it, insert_begin, insert_end);
				it += distance(insert_begin, insert_end);
				insert_begin = S.end();
				insert_end = S.begin();
			}
			it = out.begin();
		} else if ('>' == c) { // End
			if (it != out.end()) {
				if (insert_begin != insert_end) {
					it = out.insert(it, insert_begin, insert_end);
					it += distance(insert_begin, insert_end);
					insert_begin = S.end();
					insert_end = S.begin();
				}
				it = out.end();
			}
		} else if ('*' == c) { // Backspace
			if (insert_end != S.begin()) {
				--insert_end;
			}
			//if (it != out.begin()) {
			//	--it; // point to character
			//	it = out.erase(it);
			//}
		} else if ('#' == c) { // Num Lock
			num_lock = !num_lock;
			if (insert_begin != insert_end) {
				it = out.insert(it, insert_begin, insert_end);
				it += distance(insert_begin, insert_end);
				insert_begin = S.end();
				insert_end = S.begin();
			}
		} else if (!num_lock && '0' <= c && c <= '9') {
			if (insert_begin != S.end() && insert_begin != insert_end) {
				it = out.insert(it, insert_begin, insert_end);
				it += distance(insert_begin, insert_end);
				insert_begin = S.end();
				insert_end = S.begin();
			}
			; // num lock is off
		} else {
			if (insert_begin == S.end()) {
				insert_begin = s_it;
			}
			//it = out.insert(it, c);
			//++it; // next pos
			insert_end = s_it + 1;
			
		}
	}
	if (insert_begin != S.end() && insert_begin != insert_end) {
		out.insert(it, insert_begin, insert_end);
	}
	return string(out.data(), out.size());
}

string receivedTextZZZ(string S) {
	list<string::value_type> out;
	auto it = out.begin();
	auto insert_begin = S.end();
	auto insert_end = S.begin();
	bool num_lock = true;
	for (auto s_it = S.begin(); s_it != S.end(); ++s_it) {
		auto c = *s_it;
		if ('<' == c) { // Home
			if (insert_begin != insert_end) {
				it = out.insert(it, insert_begin, insert_end);
				advance(it, distance(insert_begin, insert_end));
				insert_begin = S.end();
				insert_end = S.begin();
			}
			it = out.begin();
		}
		else if ('>' == c) { // End
			if (it != out.end()) {
				if (insert_begin != insert_end) {
					it = out.insert(it, insert_begin, insert_end);
					advance(it, distance(insert_begin, insert_end));
					insert_begin = S.end();
					insert_end = S.begin();
				}
				it = out.end();
			}
		}
		else if ('*' == c) { // Backspace
			if (insert_end != S.begin()) {
				--insert_end;
			}
			//if (it != out.begin()) {
			//	--it; // point to character
			//	it = out.erase(it);
			//}
		}
		else if ('#' == c) { // Num Lock
			num_lock = !num_lock;
			if (insert_begin != insert_end) {
				it = out.insert(it, insert_begin, insert_end);
				advance(it, distance(insert_begin, insert_end));
				insert_begin = S.end();
				insert_end = S.begin();
			}
		}
		else if (!num_lock && '0' <= c && c <= '9') {
			if (insert_begin != S.end() && insert_begin != insert_end) {
				it = out.insert(it, insert_begin, insert_end);
				advance(it, distance(insert_begin, insert_end));
				insert_begin = S.end();
				insert_end = S.begin();
			}
			; // num lock is off
		}
		else {
			if (insert_begin == S.end()) {
				insert_begin = s_it;
			}
			//it = out.insert(it, c);
			//++it; // next pos
			insert_end = s_it + 1;

		}
	}
	if (insert_begin != S.end() && insert_begin != insert_end) {
		out.insert(it, insert_begin, insert_end);
	}
	return string(out.begin(), out.end());
}



//TEST_F(NewKeyboardTest, Backspace) {
//	string input = "***AAA**";
//	EXPECT_EQ(receivedText(input), "A");
//}
//
//TEST_F(NewKeyboardTest, Home) {
//	string input = "A<B<C<D<E<F";
//	EXPECT_EQ(receivedText(input), "FEDCBA");
//}
//
//TEST_F(NewKeyboardTest, HomeAndEnd) {
//	string input = "A<B<C>D<E<F";
//	EXPECT_EQ(receivedText(input), "FECBAD");
//}
//
//TEST_F(NewKeyboardTest, Home2) {
//	string input = "AA<BB<CC<DD<EE<FF";
//	EXPECT_EQ(receivedText(input), "FFEEDDCCBBAA");
//}
//
//TEST_F(NewKeyboardTest, HomeAndEnd2) {
//	string input = "AA<BB<CC>DD<EE<FF";
//	EXPECT_EQ(receivedText(input), "FFEECCBBAADD");
//}
//
//TEST_F(NewKeyboardTest, NumLock) {
//	string input = "1234567890#qwertyuiop1234567890asdfghjkl#zxcvbnm***<***";
//	EXPECT_EQ(receivedText(input), "1234567890qwertyuiopasdfghjklzxcv");
//}
//
//TEST_F(NewKeyboardTest, LongClear) {
//	EXPECT_EQ(receivedText(long_clear), "");
//}
//
//TEST_F(NewKeyboardTest, SwapBlocks) {
//	EXPECT_EQ(receivedText(swap_blocks), swap_blocks_answer);
//}

class JewelGameTest : public ::testing::Test {
protected:
	JewelGameTest() {
		for (int i = 0; i < 10000 / 4; ++i) {
			long_pairs += "aabb";
		}
	}
	string long_one_jewel = string(10000, 'a');
	string long_pairs;
};


using Collection = tuple<char, int, int>; // jewel, start, length
using Collections = list<Collection>;

int getMaxScore(string jewels) {
	Collections collections;
	vector<int> start_options;

	int min_score = 0;
	string::value_type last_jewel = -1;
	for (string::size_type start_pos = 0; start_pos < jewels.size(); ++start_pos) {
		auto jewel = jewels[start_pos];
		if (jewel == last_jewel) {
			auto& collection = collections.back();
			int& length = get<2>(collection);
			length++;
			if (length == 2) {
				start_options.push_back(get<1>(collection));
				++min_score;
			}
		}
		else {
			collections.push_back({ jewel, start_pos, 1 });
		}
		last_jewel = jewel;
	}

	vector<int> scores;
	for (auto start = collections.begin(); start != collections.end(); ++start) {
		Collections test_collections(collections.size());
		rotate_copy(collections.begin(), start, collections.end(), test_collections.begin());
		int score = 0;
		auto it = test_collections.begin();
		while (it != test_collections.end()) {
			if (get<2>(*it) < 2) {
				++it;
				continue;
			}
			it = test_collections.erase(it);
			++score;
			while (it != test_collections.begin() && it != test_collections.end()) {
				auto next_i = *it;
				auto prev_i = *prev(it);
				if (get<0>(prev_i) == get<0>(next_i)) {
					++score;
					it = test_collections.erase(prev(it));
					it = test_collections.erase(it);
				}
				else {
					break;
				}
			}
			if (it == test_collections.end()) {
				if (!test_collections.empty()) {
					it = test_collections.begin();
				}
			}
		}
		scores.push_back(score);
	}

	return *max_element(scores.begin(), scores.end());
}

//TEST_F(JewelGameTest, Test1) {
//	EXPECT_EQ(getMaxScore("abcddcbd"), 3);
//}
//
//TEST_F(JewelGameTest, Test2) {
//	EXPECT_EQ(getMaxScore("abcd"), 0);
//}
//
//TEST_F(JewelGameTest, LongChains) {
//	//EXPECT_EQ(getMaxScore("aabbaa"), 3);
//	//EXPECT_EQ(getMaxScore("abbaa"), 2);
//	//EXPECT_EQ(getMaxScore("abba"), 2);
//	//EXPECT_EQ(getMaxScore("aaaabbbbbaaaa"), 3);
//	//EXPECT_EQ(getMaxScore("aacbbaac"), 4);
//	EXPECT_EQ(getMaxScore("daaccbbaacad"), 5);
//}

//TEST_F(JewelGameTest, MaxLong) {
//	EXPECT_EQ(getMaxScore(long_one_jewel), 1);
//}
//
//TEST_F(JewelGameTest, MaxLong2) {
//	EXPECT_EQ(getMaxScore(long_pairs), 5000);
//}

int getMinConnectionChange1(vector<int> connection) {
	if (connection.size() == 1) {
		return 0;
	}
	int n_terminals = 0;
	vector<vector<int>> reverse_connections(connection.size());
	for (vector<int>::size_type start = 0; start < connection.size(); ++start) {
		if (connection[start] - 1 == start) {
			++n_terminals;
		}
		else {
			reverse_connections[connection[start] - 1].push_back(start + 1);
		}
	}
	// fix loops
	int n_loops = 0;
	unordered_set<int> middle_nodes;
	for (vector<int>::size_type start = 0; start < connection.size(); ++start) {
		int current = start;
		int next = connection[start] - 1;
		if (next <= current) {
			continue;
		}
		if (middle_nodes.count(start) == 1) {
			continue;
		}
		if (reverse_connections[current].empty()) {
			continue;
		}
		auto& back_connections = reverse_connections[current];
		while (true) {
			if (find(back_connections.begin(), back_connections.end(), next + 1) != back_connections.end()) {
				connection[current] = current + 1; // break a connection, make a terminal
				++n_loops;
				break;
			}
			else {
				current = next;
				next = connection[current] - 1;
				if (next == current) {
					break;
				}
				middle_nodes.insert(current);
			}
		}
	}
	
	if (n_loops > 0) {
		if (n_terminals > 0) {
			return n_loops + n_terminals - 1;
		}
		else {
			return n_loops;
		}
	}
	else {
		return n_terminals - 1;
	}
}

int getMinConnectionChange(vector<int> connection) {
	const int n = connection.size();
	if (n == 1) {
		return 0;
	}
	int n_terminals = 0;
	int terminal_id = -1;
	vector<bool> visited(n, false);
	vector<vector<int>> reverse_connections(n);
	for (vector<int>::size_type start = 0; start < n; ++start) {
		if (connection[start] - 1 == start) {
			++n_terminals;
			terminal_id = start;
		}
		else {
			reverse_connections[connection[start] - 1].push_back(start + 1);
		}
	}

	if (n_terminals == n) {
		return n - 1;
	}

	if (n_terminals == 1) {
		if (all_of(connection.begin(), connection.end(), [terminal_id](int x) {
			return terminal_id == x - 1;
		})) {
			return 0;
		}
	}
	// fix loops
	int n_loops = 0;
	for (vector<int>::size_type start = 0; start < n; ++start) {
		int current = start;
		int next = connection[start] - 1;
		if (next <= current) {
			continue;
		}
		if (visited[start] == true) {
			continue;
		}
		if (reverse_connections[current].empty()) {
			continue;
		}

		vector<bool> visited_nodes(n, false);
		visited_nodes[start] = true;
		while (true) {
			if (visited_nodes[next] == false) {
				visited_nodes[next] = true;
				current = next;
				next = connection[current] - 1;
				if (next == current) {
					break;
				}
				visited[current] = true;
			}
			else {
				connection[current] = current + 1; // break a connection, make a terminal
				++n_loops;
				break;
			}
		}
	}

	if (n_loops > 0) {
		if (n_terminals > 0) {
			return n_loops + n_terminals - 1;
		}
		else {
			return n_loops;
		}
	}
	else {
		return n_terminals - 1;
	}
}

class ReroutingTest : public ::testing::Test {
protected:
	ReroutingTest() {
		first_terminal_long.assign(300000, 1);
		only_terminals_long.reserve(30000);
		for (int i = 0; i < 30000; ++i) {
			only_terminals_long.push_back(i + 1);
			a_loop_long.push_back((i + 1) % 30000 + 1);
		}
	}
	vector<int> only_terminals_long;
	vector<int> first_terminal_long;
	vector<int> a_loop_long;
};

TEST_F(ReroutingTest, Test1) {
	EXPECT_EQ(getMinConnectionChange({2,3,4,1,5}), 1);  // a loop! and one prospective terminal
}

TEST_F(ReroutingTest, Test2) {
	EXPECT_EQ(getMinConnectionChange({1,2,3,4}), 3); // no connections
}

TEST_F(ReroutingTest, MyTest1) {
	EXPECT_EQ(getMinConnectionChange({ 1,2,3,4,4,4 }), 3);
}

TEST_F(ReroutingTest, MyTest2) {
	EXPECT_EQ(getMinConnectionChange({ 2,3,4,4,4,4 }), 0);
}

TEST_F(ReroutingTest, MyTest3) {
	EXPECT_EQ(getMinConnectionChange({ 2,3,4,4,4 }), 0);
}

TEST_F(ReroutingTest, MyTest4) {
	EXPECT_EQ(getMinConnectionChange({ 2,3,4,5,5 }), 0);
}

TEST_F(ReroutingTest, MyTest5) {
	EXPECT_EQ(getMinConnectionChange({ 1,1,4,4,4 }), 1);
}

TEST_F(ReroutingTest, WhatCanFail1) {
	EXPECT_EQ(getMinConnectionChange({ 1 }), 0);
}

TEST_F(ReroutingTest, WhatCanFail2) {
	EXPECT_EQ(getMinConnectionChange({ 1, 1 }), 0);
	EXPECT_EQ(getMinConnectionChange({ 1, 2 }), 1); // no connections
	EXPECT_EQ(getMinConnectionChange({ 2, 2 }), 0);
	EXPECT_EQ(getMinConnectionChange({ 2, 1 }), 1); // a loop!
}

TEST_F(ReroutingTest, WhatCanFail3) {
	EXPECT_EQ(getMinConnectionChange({ 1,1,1,1,5,5,5,5 }), 1);
	EXPECT_EQ(getMinConnectionChange({ 1,1,1,1,5,5,1,5 }), 1);
	EXPECT_EQ(getMinConnectionChange({ 1,5,5,1,5,5,5,5 }), 1);
	EXPECT_EQ(getMinConnectionChange({ 2,5,5,1,5,5,5,5 }), 0);
}

TEST_F(ReroutingTest, WhatCanFail4) {
	EXPECT_EQ(getMinConnectionChange({ 2,1,3 }), 1); // a loop!
	EXPECT_EQ(getMinConnectionChange({ 2,1,2 }), 1); // a loop!
	EXPECT_EQ(getMinConnectionChange({ 2,3,1 }), 1); // a loop!
}

TEST_F(ReroutingTest, WhatCanFail5) {
	EXPECT_EQ(getMinConnectionChange({ 2,1,4,3,6,5,7 }), 3);
	EXPECT_EQ(getMinConnectionChange({ 2,1,4,3,6,5,7,8 }), 4);
}

TEST_F(ReroutingTest, TimeoutTest) {
	EXPECT_EQ(getMinConnectionChange(first_terminal_long), 0);
}

TEST_F(ReroutingTest, TimeoutTest2) {
	EXPECT_EQ(getMinConnectionChange(only_terminals_long), only_terminals_long.size()-1);
}

TEST_F(ReroutingTest, TimeoutTest3) {
	EXPECT_EQ(getMinConnectionChange(a_loop_long), 1);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();

	//ofstream out("long_test.txt");
	//for (int i = 0; i < 30000; ++i) {
	//	out << i << " ";
	//}
	//out << "\n";
	//out.close();

	//int n = 30000;
	//int n_terminals = 0;
	//auto start = std::chrono::high_resolution_clock::now();
	//vector<int> connection{ istream_iterator<int>{cin}, istream_iterator<int>{} };
	//
	//auto finish = std::chrono::high_resolution_clock::now();
	//std::chrono::duration<double> elapsed = finish - start;
	//std::cout << "Elapsed time: " << elapsed.count() << " s\n";
}
