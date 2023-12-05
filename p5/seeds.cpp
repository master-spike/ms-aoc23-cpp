
#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <istream>
#include <algorithm>
#include <unordered_map>
#include <regex>
#include <utility>
#include <optional>
#include <iostream>

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

std::string load_file_input(const std::string& path) {
    std::fstream file(path);
    std::string input_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return input_data;
}

struct seeds_map_entry {
    uint64_t from;
    uint64_t to;
    uint64_t interval;
    seeds_map_entry(uint64_t _f, uint64_t _t, uint64_t _i)
    : from(_f), to(_t), interval(_i)
    {}
};

template<class InputIt>
uint64_t compute_next(InputIt entries_begin, InputIt entries_last, uint64_t input_val) {
    for (; entries_begin != entries_last; ++entries_begin) {
        if (entries_begin->from <= input_val && entries_begin->from + entries_begin->interval > input_val) {
            return entries_begin->to + input_val - entries_begin->from;
        }
    }
    return input_val;
}

std::vector<seeds_map_entry>  generate_map_entries(std::vector<std::string_view>::iterator begin, std::vector<std::string_view>::iterator end) {
    std::regex rgx("([0-9]+) ([0-9]+) ([0-9]+)");
    std::match_results<std::string_view::iterator> mr;
    std::vector<seeds_map_entry> ret;
    while (begin != end && std::regex_match(begin->begin(), begin->end(), mr, rgx)) {
        std::string_view line = *begin;
        auto [t, l1] = get_next_number(line).value();
        auto [f, l2] = get_next_number(l1).value();
        auto [i, l3] = get_next_number(l2).value();
        ret.emplace_back(f,t,i);
        ++begin;
    }
    return ret;
}

int main() {
    std::vector<uint64_t> init_seeds;
    std::string input_file_str = load_file_input("input.txt");
    std::vector<std::string_view> str_lines = split(input_file_str, '\n');
    std::string_view first_line = *str_lines.begin();
    for (auto x = get_next_number(first_line); x; x = get_next_number(x.value().second)) {
        init_seeds.emplace_back(x.value().first);
    }
    std::vector<std::pair<uint64_t, uint64_t>> init_seeds2;
    for (int i = 0; i + 1< init_seeds.size(); i += 2) {
        init_seeds2.emplace_back(init_seeds[i], init_seeds[i+1]);
    }
    auto it = str_lines.begin() + 1;
    std::vector<std::vector<seeds_map_entry>> entries_table;
    while (it != str_lines.end()) {
        while(it != str_lines.end() && it->find(':') == std::string_view::npos) {
            ++it;      
        }
        if (it == str_lines.end()) break;
        ++it;
        entries_table.emplace_back(generate_map_entries(it, str_lines.end()));
    }
    auto compute_land = [&entries_table](uint64_t v) -> std::pair<uint64_t,uint64_t> {
        uint64_t q = v;
        for (auto const& col : entries_table) {
            q = compute_next(col.cbegin(), col.cend(), q);
        }
        return std::make_pair(v, q);
    };

    auto compute_land2 = [&entries_table, compute_land](std::pair<uint64_t, uint64_t> seed_range) {
        uint64_t min = UINT64_MAX;
        for (uint64_t r = seed_range.first; r < seed_range.first + seed_range.second; ++r) {
            min = std::min(compute_land(r).second, min);
        }
        return min;
    };

    auto compare_second = [](std::pair<uint64_t,uint64_t> const& lhs, std::pair<uint64_t,uint64_t> const& rhs) {
        return lhs.second < rhs.second;
    };

    std::vector<std::pair<uint64_t,uint64_t>> seed_lands;
    std::transform(init_seeds.begin(), init_seeds.end(), std::back_inserter(seed_lands), compute_land);
    std::cout << std::min_element(seed_lands.begin(), seed_lands.end(), compare_second)->second << std::endl;
    uint64_t min = UINT64_MAX;
    for (auto const& p : init_seeds2) {
        min = std::min(compute_land2(p), min);
        std::cout << p.first << "," << p.second << " computed" << std::endl;
    }
    std::cout << min << std::endl;
}
