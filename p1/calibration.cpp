
#include <vector>
#include <string>
#include <string_view>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <streambuf>
#include <utility>
#include <optional>

int digits (std::string_view str) {

    auto is_digit = [](char c) -> bool {
        return (c >= '0') & (c <= '9');
    };
    auto to_digit = [](char c) -> int {
        return c - '0';
    };

    
    auto first = std::find_if(str.begin(), str.end(), is_digit);
    if (first == str.end()) return 0;
    auto last = std::find_if(str.rbegin(), str.rend(), is_digit);
    
    return to_digit(*first) * 10 + to_digit(*last);

}

int digits2 (std::string_view str) {
    static const std::array<std::pair<std::string, int>, 10> digit_str{
        std::make_pair("zero", 0),
        std::make_pair("one", 1),
        std::make_pair("two", 2),
        std::make_pair("three", 3),
        std::make_pair("four", 4),
        std::make_pair("five", 5),
        std::make_pair("six", 6),
        std::make_pair("seven",7),
        std::make_pair("eight",8),
        std::make_pair("nine",9) 
    };
    auto get_digit = [](std::string_view subview) -> std::optional<int> {
        if (subview.length() == 0) return std::nullopt;
        if ('0' <= subview.at(0) && subview.at(0) <= '9') return subview.at(0) - '0';
        for (auto const& [k,v] : digit_str) {
            if (subview.substr(0, k.length()) == k) {
                return v;
            }
        }
        return std::nullopt;
    };
    int first_digit = -1;
    int last_digit = -1;
    for (std::size_t i = 0; i < str.length(); ++i) {
        auto digit_here = get_digit(str.substr(i));
        if (digit_here) {
            if (first_digit == -1) first_digit = digit_here.value();
            last_digit = digit_here.value();
        }
    }
    return first_digit * 10 + last_digit;
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

int main(int argc, char** argv) {
    std::fstream file("input.txt");
    std::string input_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    auto substrings = split(input_data, '\n');
    int res = 0;
    int res2 = 0;
    for (auto sv : substrings) {
        std::cout << sv << " | " << digits(sv) << "," << digits2(sv) << std::endl;
        res += digits(sv);
        res2 += digits2(sv);
    }
    std::cout << res << std::endl;
    std::cout << res2 << std::endl;
    return 0;
}