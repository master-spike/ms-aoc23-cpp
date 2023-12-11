#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <istream>
#include <utility>
#include <optional>
#include <unordered_map>
#include <set>



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

std::set<int> expanded_rows(std::vector<std::string_view> const& universe) {
    std::set<int> res;
    for (int i = 0; i < universe.size(); ++i) {
        if (std::find(universe[i].begin(), universe[i].end(), '#') == universe[i].end()) res.emplace_hint(res.end(), i);
    }
    return res;
}

std::set<int> expanded_cols(std::vector<std::string_view> const& universe) {
    std::set<int> res;
    for (int i = 0; i < universe[0].size(); ++i) {
        bool flag = true;
        for (int j = 0; j < universe.size(); ++j) {
            if (universe[j][i] == '#') {
                flag = false; break;
            }
        }
        if (flag) res.emplace_hint(res.end(), i);
    }
    return res;
}

uint64_t distance(std::pair<int,int> g1, std::pair<int,int> g2, std::set<int> const& cols, std::set<int> const& rows, std::ptrdiff_t scale = 1LL) {
    uint64_t mh_dist = std::abs(g1.first - g2.first) + std::abs(g1.second - g2.second);
    auto it_c = cols.lower_bound(std::min(g1.second, g2.second));
    auto jt_c = cols.lower_bound(std::max(g1.second, g2.second));
    mh_dist += std::distance(it_c, jt_c) * scale;
    auto it_r = rows.lower_bound(std::min(g1.first, g2.first));
    auto jt_r = rows.upper_bound(std::max(g1.first, g2.first));
    mh_dist += std::distance(it_r, jt_r) * scale;
    return mh_dist;
}

std::vector<std::pair<int,int>> galaxies(std::vector<std::string_view> const& universe) {
    std::vector<std::pair<int,int>> galax;
    for (int i = 0; i < universe.size(); ++i) {
        for (int j = 0; j < universe[i].size(); ++j) {
            if (universe[i][j] == '#') galax.emplace_back(i,j);
        }
    }
    return galax;
}



int main(int argc, char** argv) {
    std::string input_str = load_file_input("input.txt");
    auto universe = split(input_str, '\n');
    auto cols = expanded_cols(universe);
    auto rows = expanded_rows(universe);
    auto galax = galaxies(universe);
    uint64_t total_dist = 0;
    uint64_t total_dist_2 = 0;
    for (auto it = galax.begin(); it != galax.end(); ++it) {
        for (auto jt = it + 1; jt != galax.end(); ++jt) {
            auto v = distance(*it, *jt, cols, rows);
            auto w = distance(*it, *jt, cols, rows, 999999);
            std::cout << w << std::endl;
            total_dist += v;
            total_dist_2 += w;
        }
    }
    std::cout << total_dist << std::endl;
    std::cout << total_dist_2 << std::endl;
}
