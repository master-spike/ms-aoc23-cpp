#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <sstream>
#include <map>
#include <array>
#include <utility>
#include <algorithm>
#include <numeric>
#include <cstdint>

std::pair<int, std::string_view> get_next_number(std::string_view str) {
    auto is_digit = [](char c) { return c >= '0' && c <= '9';};
    while(str.length() > 0 && !is_digit(str.at(0))) {
        str = str.substr(1);
    }
    int val = 0;
    while (str.length() > 0 && is_digit(str.at(0))) {
        val *= 10;
        val += static_cast<int>(str.at(0) - '0');
        str = str.substr(1);
    }
    return std::make_pair(val, str);
}

std::string_view drop_colon(std::string_view sv) {
    auto it = std::find(sv.begin(), sv.end(), ':');
    if (it == sv.end()) return std::string_view(it,sv.end());
    return std::string_view(it + 1, sv.end());
}

std::vector<std::string_view> split(std::string_view str, char c) {
    auto it = str.begin();
    while (it != str.end() && *it == c) ++it;
    auto jt = it;
    std::vector<std::string_view> output;
    while (it != str.end()) {
        while (jt != str.end() && *jt != c) ++jt;
        if (it != jt) output.emplace_back(it, jt);
        while (jt != str.end() && *jt == c) ++jt;
        it = jt;
    }
    return output;
}

int score(std::string_view line) {
    line = drop_colon(line);
    std::vector<std::string_view> vec = split(line, '|');
    if (vec.size() != 2) return 0;
    std::vector<std::string_view> nums_raw_l = split(vec[0], ' ');
    std::vector<std::string_view> nums_raw_r = split(vec[1], ' ');
    std::vector<int> winning;
    int score = 1;
    for (auto sv : nums_raw_l) {
        winning.push_back(get_next_number(sv).first);
    }
    for (auto sv : nums_raw_r) {
        if (std::find(winning.begin(), winning.end(), get_next_number(sv).first) == winning.end()) continue;
        score <<= 1;
    }
    score >>= 1;
    return score;
}

int num_wins(std::string_view line) {
    int score = 0;
    line = drop_colon(line);
    std::vector<std::string_view> vec = split(line, '|');
    if (vec.size() != 2) return 0;
    std::vector<std::string_view> nums_raw_l = split(vec[0], ' ');
    std::vector<std::string_view> nums_raw_r = split(vec[1], ' ');
    std::vector<int> winning;
    for (auto sv : nums_raw_l) {
        winning.push_back(get_next_number(sv).first);
    }
    for (auto sv : nums_raw_r) {
        if (std::find(winning.begin(), winning.end(), get_next_number(sv).first) == winning.end()) continue;
        ++score;
    }
    return score;
}

int main(int argc, char** argv) {
    std::fstream file("input.txt");
    std::string input_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    auto substrings = split(input_data, '\n');
    int total_score = 0;
    for (auto line : substrings) {
        int score_l = score(line);
        std::cout << line << " \\ score=" << score_l << std::endl;
        total_score += score_l;
    }
    std::vector<std::uint64_t> sc_counts(substrings.size(), 1);
    for (std::size_t i = 0; i < substrings.size(); ++i) {
        std::uint64_t num_scs = sc_counts[i];
        std::uint64_t wins = num_wins(substrings[i]);
        if (num_scs == 0) continue;
        for (auto it = sc_counts.begin() + i + 1; it < sc_counts.begin() + i + wins + 1; ++it) {
            if (it == sc_counts.end()) break;
            *it += num_scs;
        }
    }
    std::cout << total_score << std::endl;
    std::cout << std::accumulate(sc_counts.begin(), sc_counts.end(), 0ULL) << std::endl;
    return 0;
}
