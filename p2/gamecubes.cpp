#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <sstream>
#include <map>
#include <array>
#include <utility>

enum class color {
    INVALID,
    RED,
    GREEN,
    BLUE
};

static const std::array<std::pair<std::string, color>, 3> color_table {
    std::make_pair("red", color::RED),
    std::make_pair("green", color::GREEN),
    std::make_pair("blue", color::BLUE)
};

std::pair<color, std::string_view> get_next_color(std::string_view str) {
    while (str.length() > 0) {
        for (auto const& [k,v] : color_table) {
            if (str.substr(0, k.length()) == k) return std::make_pair(v, str.substr(k.length()));
        }
        str = str.substr(1);
    }
    return std::make_pair(color::INVALID, str);
}

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

struct res_data {
    int red = 0;
    int green = 0;
    int blue = 0;

    res_data() = default;
    res_data(int _r, int _g, int _b) : red(_r), green(_g), blue(_b) {}

    res_data(std::string_view str) {
        while (!str.empty()) {
            auto [v,s1] = get_next_number(str);
            str = s1;
            auto [c,s2] = get_next_color(str);
            str = s2;
            switch (c)
            {
            case color::RED:
                red = v;
                break;
            case color::GREEN:
                green = v;
                break;
            case color::BLUE:
                blue = v;
                break;
            default:
                break;
            }
        }
    };

    bool compare(res_data const& other) const {
        return other.red <= red && other.green <= green && other.blue <= blue;
    }

    res_data combine(res_data other) const {
        other.red = std::max(other.red, red);
        other.green = std::max(other.green, green);
        other.blue = std::max(other.blue, blue);
        return other;
    }

    int power() {
        return red * green * blue;
    }
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

int main(int argc, char** argv) {
    std::fstream file("input.txt");
    std::string input_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    auto substrings = split(input_data, '\n');
    int res = 0;
    int power = 0;
    res_data bag(12, 13, 14);
    for (auto sv : substrings) {
        auto [id, g_result_str] = get_next_number(sv);
        bool possible = true;
        res_data min_bag;
        for (std::string_view game : split(g_result_str, ';')) {
            res_data game_res(game);
            possible &= bag.compare(game_res);
            min_bag = game_res.combine(min_bag);
        }
        std::cout << sv << " || " << id << " " << possible << " | " << min_bag.power() << std::endl;
        power += min_bag.power();
        if (possible) res += id;
    }
    std::cout << "RESULT : " << res << std::endl;
    std::cout << "sum_power : " << power << std::endl;
    return 0;
}
