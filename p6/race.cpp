#include <vector>
#include <iostream>
#include <string>
#include <string_view>
#include <algorithm>
#include <istream>
#include <fstream>
#include <iterator>
#include <optional>
#include <utility>
#include <cstdint>
#include <numeric>

std::optional<std::pair<uint64_t, std::string_view>> get_next_number(std::string_view str) {
    auto is_digit = [](char c) { return c >= '0' && c <= '9';};
    while(str.length() > 0 && !is_digit(str.at(0))) {
        str = str.substr(1);
    }
    long val = 0;
    if (str.length() == 0) return std::nullopt;
    while (str.length() > 0 && is_digit(str.at(0))) {
        val *= 10;
        val += static_cast<uint64_t>(str.at(0) - '0');
        str = str.substr(1);
    }
    return std::make_pair(val, str);
}

std::vector<uint64_t> get_all_numbers(std::string_view str) {
    std::vector<uint64_t> ret;
    for (auto opt = get_next_number(str); opt; opt = get_next_number(opt->second)) {
        ret.emplace_back(opt->first);
    }
    return ret;
}

std::vector<std::pair<uint64_t, uint64_t>> zip_vec_u64(const std::vector<uint64_t>& l, const std::vector<uint64_t>& r) {
    std::vector<std::pair<uint64_t, uint64_t>> ret;
    for (auto it = l.cbegin(), jt = r.cbegin(); it != l.cend() && jt != r.cend(); ++it, ++jt) {
        ret.emplace_back(*it, *jt);
    }
    return ret;
}

uint64_t compute_ways(std::pair<uint64_t, uint64_t> time_dist) {
    uint64_t speed_lb = time_dist.second / time_dist.first;
    uint64_t min_hold = speed_lb;
    if (min_hold >= time_dist.first) return 0;
    while ((time_dist.first - min_hold) * (min_hold) < time_dist.second) {
        ++min_hold;
        if (min_hold >= time_dist.first) return 0;
    }
    std::cout << "{" << time_dist.first << "," << time_dist.second << "} : min_hold=" << min_hold << std::endl;
    uint64_t l = min_hold;
    uint64_t r = time_dist.first;

    while (l + 1 < r) {
        uint64_t m = l + (r - l) / 2;
        if ((time_dist.first - m) * m < time_dist.second) r = m;
        else l = m;
    }
    return r - min_hold;
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

std::string load_file_input(const std::string& path) {
    std::fstream file(path);
    std::string input_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return input_data;
}

int main(int argc, char **argv) {
    auto str_input = load_file_input("input.txt");
    auto lines = split(str_input, '\n');
    std::vector<std::pair<uint64_t, uint64_t>> race_records = zip_vec_u64(get_all_numbers(lines[0]), get_all_numbers(lines[1]));
    std::cout << std::accumulate(race_records.begin(), race_records.end(), 1, [](uint64_t v, std::pair<uint64_t, uint64_t> const& p){
        return v * compute_ways(p);
    }) << std::endl;
    std::string num1;
    std::string num2;
    for (char c : lines[0]) {
        if (c <= '9' && c >= '0') num1 += c;
    }
    for (char c : lines[1]) {
        if (c <= '9' && c >= '0') num2 += c;
    }
    std::pair<uint64_t, uint64_t> big_race = std::make_pair(get_next_number(num1)->first, get_next_number(num2)->first);
    std::cout << compute_ways(big_race) << std::endl;
    return 0;
}

