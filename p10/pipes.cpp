#include <fstream>
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <string_view>
#include <algorithm>
#include <array>
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

const std::array<char, 4> up = {'|', 'F', '7', 'S'};
const std::array<char, 4> down = {'|', 'J', 'L', 'S'};
const std::array<char, 4> left = {'-', 'F', 'L', 'S'};
const std::array<char, 4> right = {'-', 'J', '7', 'S'};


using tilepos = std::pair<uint64_t, uint64_t>;
std::pair<tilepos, tilepos> adj_loop(std::vector<std::string_view> const& grid, tilepos tile) {
    tilepos t1(-1, -1);
    tilepos t2(-1, -1);
    if (tile.first != 0 && std::find(down.begin(), down.end(), grid[tile.first][tile.second]) != down.end()) {
        if (std::find(up.begin(), up.end(), grid[tile.first - 1][tile.second]) != up.end()) {
            t1 = std::make_pair(tile.first - 1, tile.second);
            std::swap(t1, t2);
        }
    }
    if (tile.first != grid.size() - 1 && std::find(up.begin(), up.end(), grid[tile.first][tile.second]) != up.end()) {
        if (std::find(down.begin(), down.end(), grid[tile.first + 1][tile.second]) != down.end()) {
            t1 = std::make_pair(tile.first + 1, tile.second);
            std::swap(t1, t2);
        }
    }
    if (tile.second != 0 && std::find(right.begin(), right.end(), grid[tile.first][tile.second]) != right.end()) {
        if (std::find(left.begin(), left.end(), grid[tile.first][tile.second - 1]) != left.end()) {
            t1 = std::make_pair(tile.first, tile.second - 1);
            std::swap(t1, t2);
        }
    }
    if (tile.second != grid[tile.first].size() - 1 && std::find(left.begin(), left.end(), grid[tile.first][tile.second]) != left.end()) {
        if (std::find(right.begin(), right.end(), grid[tile.first][tile.second + 1]) != right.end()) {
            t1 = std::make_pair(tile.first, tile.second + 1);
            std::swap(t1, t2);
        }
    }

    return std::make_pair(t1, t2);
}

std::vector<tilepos> create_loop(std::vector<std::string_view> const& grid, tilepos initial_pos) {
    std::vector<tilepos> loop;
    std::set<tilepos> visited;
    loop.push_back(initial_pos);
    visited.insert(initial_pos);
    tilepos current = initial_pos;
    while(true) {
        auto neighbours = adj_loop(grid, current);
        if (!visited.contains(neighbours.first)) {
            loop.push_back(neighbours.first);
            visited.insert(neighbours.first);
            current = neighbours.first;
            continue;
        }
        if (!visited.contains(neighbours.second)) {
            loop.push_back(neighbours.second);
            visited.insert(neighbours.second);
            current = neighbours.second;
            continue;
        }
        break;
    }
    auto dir = [&loop](std::size_t s) {
        if (s == 0 || s == loop.size()) {
            return std::make_pair(static_cast<int>(loop[0].first) - static_cast<int>(loop.back().first), static_cast<int>(loop[0].second) - static_cast<int>(loop.back().second));
        }
        return std::make_pair(static_cast<int>(loop[s].first) - static_cast<int>(loop[s-1].first), static_cast<int>(loop[s].second) - static_cast<int>(loop[s-1].second));
    };

    auto align = [](std::pair<int,int> p1, std::pair<int,int> p2){
        if (std::abs(p1.first) + std::abs(p1.second) != 1) std::cout << "BAD DIRECTION" << std::endl;
        if (p1.first == 1) return p2;
        if (p1.first == -1) return std::make_pair(-p2.first, -p2.second);
        if (p1.second == 1) return std::make_pair(p2.second, -p2.first);
        if (p1.second == -1) return std::make_pair(-p2.second, p2.first);
        return p2;
    };
    int rot = 0;
    for (std::size_t s = 0; s < loop.size(); ++s) {
        auto dir_in = dir(s);
        auto dir_out = dir(s+1);
        auto dir_aligned = align(dir_in, dir_out);
        rot += dir_aligned.second;
    }
    if (rot < 0) std::reverse(loop.begin(), loop.end());
    return loop;
}

tilepos find_S(std::vector<std::string_view> const& grid) {
    for (auto it = grid.begin(); it != grid.end(); ++it) {
        auto jt = std::find(it->begin(), it->end(), 'S');
        if (jt != it->end()) return std::make_pair(std::distance(grid.begin(), it), std::distance(it->begin(), jt));
    }
    return std::make_pair(-1, -1);
}

int flood_fill_ground(std::vector<std::string_view> const& grid, tilepos init, std::set<tilepos>& visited) {
    std::vector<tilepos> stack;
    if (!visited.contains(init)) {
        stack.push_back(init);
    }
    else {
        return 0;
    }
    int count = 0;
    visited.insert(init);
    auto neighbours = [&grid](tilepos p) {
        std::vector<tilepos> result;
        if (p.first != 0) {
            result.emplace_back(p.first - 1, p.second);
        }
        if (p.first != grid.size() - 1) {
            result.emplace_back(p.first + 1, p.second);
        }
        if (p.second != 0) {
            result.emplace_back(p.first, p.second - 1);
        }
        if (p.second != grid[p.first].size() - 1) {
            result.emplace_back(p.first, p.second + 1);
        }
        return result;
    };
    while(!stack.empty()) {
        tilepos p = stack.back();
        stack.pop_back();
        ++count;
        visited.insert(p);
        for (auto n : neighbours(p)) {
            if (visited.contains(n)) continue;
            visited.insert(n);
            stack.emplace_back(n);
        }
    }
    return count;
}

int main(int argc, char** argv) {
    std::string str_input = load_file_input("input.txt");
    auto lines = split(str_input, '\n');
    const tilepos start = find_S(lines);
    std::set<tilepos> visited;
    std::vector<tilepos> stack;
    stack.emplace_back(start);
    while(!stack.empty()) {
        auto pos = stack.back();
        stack.pop_back();
        if (visited.find(pos) != visited.end()) continue;
        visited.insert(pos);
        auto pair = adj_loop(lines, pos);
        stack.emplace_back(pair.first);
        stack.emplace_back(pair.second);
    }
    std::cout << visited.size() / 2 << std::endl;
    std::cout << std::endl;
    int ground_cnt = 0;
    tilepos p(-1,-1);
    tilepos q(-1,-1);
    int parity = 0;
    for (auto it = visited.begin(); it != visited.end(); ++it) {
        if (it->first != p.first) {
            p = *it;
            q = *it;
            parity = 1;
            continue;
        }
        if (lines[it->first][it->second] != '-' &&
            !(lines[it->first][it->second] == 'J' && lines[q.first][q.second] == 'F') &&
            !(lines[it->first][it->second] == '7' && lines[q.first][q.second] == 'L')) ++parity;
        if (parity % 2 == 0) {
            ground_cnt += it->second - p.second - 1;
        }
        p = *it;
        if (lines[it->first][it->second] != '-') q = *it;
    }
    std::cout << ground_cnt << std::endl;


}
