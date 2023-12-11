#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>
#include <istream>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <utility>
#include <optional>
#include <set>
#include <numeric>

int svtoi(std::string_view input)
{
    int acc = 0;
    for (char c : input) {
        if (c < '0' || c > '9') return acc;
        acc *= 10;
        acc += c - '0';
    }
    return acc;
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

std::optional<std::pair<uint64_t, std::pair<uint64_t, uint64_t>>> parse_line(std::string_view line) {
    int num_chars = 0;
    uint64_t a = 0;
    uint64_t b = 0;
    uint64_t c = 0;
    uint64_t acc = 0;
    std::cout << line << std::endl;
    for (char d : line) {
        if (d >= 'A' && d <= 'Z') {
            acc *= 26;
            acc += d - 'A';
            ++num_chars;
            std::cout << acc << " " << num_chars <<std::endl;
            if (num_chars == 3) {
                a = acc;
                acc = 0;
                std::cout << "SETTING A : " << a << std::endl;
            }
            if (num_chars == 6) {
                b = acc;
                acc = 0;
            }
            if (num_chars == 9) {
                c = acc;
                acc = 0;
            }
        }
    }
    if (num_chars == 9) {
        std::cout << a << " : {" << b << "," << c << "}" << std::endl;
        return std::make_pair(a, std::make_pair(b,c));
    }
    return std::nullopt;
}

uint64_t dist_to_z(std::unordered_map<uint64_t, std::pair<uint64_t, uint64_t>> const& map, std::string_view lr, uint64_t start) {
    uint64_t dist = 0;
    uint64_t current = start;
    auto it = lr.begin();
    while (current % 26 != 25) {
        if (*it == 'L') current = map.find(current)->second.first;
        if (*it == 'R') current = map.find(current)->second.second;
        ++it;
        if (it == lr.end()) it = lr.begin();
        ++dist;
    }
    return dist;
}

int main() {
    std::string str_input = load_file_input("input.txt");
    std::unordered_map<uint64_t, std::pair<uint64_t, uint64_t>> map;
    std::vector<std::string_view> input_lines = split(str_input, '\n');
    for (auto it = input_lines.begin() + 1; it < input_lines.end(); ++it) {
        auto p = parse_line(*it);
        if (p) {
            map.emplace(*p);
            std::cout << p->first << " : {" << p->second.first << "," << p->second.second << "}" << std::endl;
        }
    }

    int distance = 0;
    constexpr uint64_t src = 0;
    constexpr uint64_t dest = 17575ULL;
    uint64_t current = src;
    std::string_view lr_str = input_lines.at(0);
    auto it = lr_str.begin();
    std::cout << lr_str << std::endl;
    while (current != dest) {
        std::cout << current << " " << *it << std::endl;
        if (*it == 'L') {
            current = map.find(current)->second.first;
            ++distance;
        }
        if (*it == 'R') {
            current = map.find(current)->second.second;
            ++distance;
        }
        ++it;
        if (it == lr_str.end()) it = lr_str.begin();
    }
    std::cout << "Distance: " << distance << std::endl;

    std::vector<uint64_t> current_nodes;
    for (auto p : map) {
        if (p.first % 26 == 0) current_nodes.push_back(p.first);
    }
    distance = 0;
    it = lr_str.begin();
    std::vector<uint64_t> distances;
    std::transform(current_nodes.begin(), current_nodes.end(), std::back_inserter(distances), std::bind_front(dist_to_z, map, lr_str));
    for (auto v : distances) {
        std::cout << v << std::endl;
    }
    std::cout << std::accumulate(distances.begin(), distances.end(), 1ULL, std::lcm<uint64_t, uint64_t>) << std::endl;
    return 0;
}