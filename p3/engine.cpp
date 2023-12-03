#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <sstream>
#include <map>
#include <array>
#include <utility>

struct number_position {
    int number;
    std::size_t line;
    std::size_t begin_char;
    std::size_t end_char;
    number_position(int n, std::size_t l, std::size_t b, std::size_t e)
    : number(n), line(l), begin_char(b), end_char(e) {}
};

std::vector<number_position> find_numbers(std::vector<std::string_view> const& grid) {
    auto is_digit = [](char c) { return c <= '9' && c >= '0';};
    bool currently_in_num = false;
    int current_num = 0;
    std::vector<number_position> res;
    for (std::size_t i = 0; i < grid.size(); ++i) {
        std::string_view line = grid.at(i);
        std::size_t fd = 0;
        for (std::size_t j = 0; j <= line.length(); ++j) {
            if (j == line.length()) {
                if (currently_in_num) res.emplace_back(current_num, i, fd, j);
                currently_in_num = false;
                current_num = 0;
                continue;
            }
            if (is_digit(line[j])) {
                if (!currently_in_num) {
                    currently_in_num = true;
                    fd = j;
                }
                current_num *= 10;
                current_num += line[j] - '0';
            }
            if (!is_digit(line[j]) && currently_in_num) {
                currently_in_num = false;
                res.emplace_back(current_num, i, fd, j);
                current_num = 0;
            }
        }

    }
    return res;
}

bool has_adjecent_symbol(number_position const& np, std::vector<std::string_view> const& grid) {
    auto is_digit = [](char c) { return c <= '9' && c >= '0';};
    auto is_symbol = [is_digit](char c) {return !is_digit(c) && c != '.';};
    std::size_t l1 = std::min(np.line, np.line - 1);
    std::size_t l2 = std::min(grid.size() - 1, np.line + 1);
    std::size_t c1 = std::min(np.begin_char, np.begin_char - 1);
    for (std::size_t l = l1; l <= l2; ++l) {
        std::size_t c2 = std::min(np.end_char+1, grid[l].length());
        for (char c : grid[l].substr(c1, c2 - c1)) {
            if (is_symbol(c)) return true;
        }

    }
    return false;
}

std::vector<std::pair<std::size_t, std::size_t>> find_adjacent_stars(number_position const& np, std::vector<std::string_view> const& grid) {
    auto is_star = [](char c) { return c == '*';};
    std::size_t l1 = std::min(np.line, np.line - 1);
    std::size_t l2 = std::min(grid.size() - 1, np.line + 1);
    std::size_t c1 = std::min(np.begin_char, np.begin_char - 1);
    std::vector<std::pair<std::size_t, std::size_t>> res;
    for (std::size_t l = l1; l <= l2; ++l) {
        std::size_t c2 = std::min(np.end_char+1, grid[l].length());
        for (std::size_t i = c1; i < c2; ++i) {
            if (is_star(grid[l][i])) res.emplace_back(l, i);
        }
    }
    return res;
};

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

int main(int argc, char** argv) {
    std::string input_data = load_file_input("input.txt");
    auto substrings = split(input_data, '\n');
    auto number_positions = find_numbers(substrings);
    std::cout << input_data << std::endl;
    int res = 0;
    std::map<std::pair<std::size_t, std::size_t>, std::vector<int>> star_adjs;
    auto add_adjacency = [&star_adjs](std::pair<std::size_t, std::size_t> star_pos, int value) {
        auto it = star_adjs.find(star_pos);
        if (it == star_adjs.end()) {
            star_adjs.emplace(star_pos, std::vector(1, value));
            return;
        }
        it->second.emplace_back(value);
    };
    for (auto n : number_positions) {
        bool has_adj = has_adjecent_symbol(n, substrings);
        std::cout << n.number << " at l:" << n.line << " " << n.begin_char << "-" << n.end_char << " has adj - " << has_adj << std::endl;
        if (has_adj) res += n.number;
        for (auto sp : find_adjacent_stars(n, substrings)) {
            add_adjacency(sp, n.number);
        }
    }
    int res2 = 0;
    for (auto const& [sp, vec] : star_adjs) {
        std::cout << "star at " << sp.first << "," << sp.second << " num adjs = " << vec.size() << std::endl;
        if (vec.size() == 2) {
            std::cout << "   - power=" << vec[0] << "*" << vec[1] << std::endl; 
            res2 += vec[0] * vec[1];
        }
    }
    std::cout << res << std::endl;
    std::cout << res2 << std::endl;
    return 0;
}
