#include <fstream>
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <string_view>
#include <map>
#include <algorithm>


int svtoi(std::string_view input)
{
    int acc = 0;
    int sgn = 1;
    if (input[0] == '-') {
        sgn = -1;
        input = input.substr(1);
    }
    for (char c : input) {
        if (c < '0' || c > '9') return acc;
        acc *= 10;
        acc += c - '0';
    }
    return acc * sgn;
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

int extrapolate(const std::vector<int>& vec) {
    std::vector<std::vector<int>> vecs;
    vecs.push_back(vec);
    while(!std::all_of(vecs.back().begin(), vecs.back().end(), [](int val) {
        return val == 0;
    })) {
        std::vector<int> next;
        for (auto it = vecs.back().begin(); it + 1 != vecs.back().end(); ++it) {
            next.emplace_back(*(it+1) - *it);
        }
        vecs.emplace_back(std::move(next));
    };
    while(vecs.size() > 1) {
        int v = vecs.back().back();
        vecs.pop_back();
        vecs.back().push_back(vecs.back().back() + v);
    }
    return vecs.back().back();
}

int extrapolate_reverse(const std::vector<int>& vec) {
    std::vector<std::vector<int>> vecs;
    vecs.emplace_back(vec.rbegin(), vec.rend());
    while(!std::all_of(vecs.back().begin(), vecs.back().end(), [](int val) {
        return val == 0;
    })) {
        std::vector<int> next;
        for (auto it = vecs.back().begin(); it + 1 != vecs.back().end(); ++it) {
            next.emplace_back(*it - *(it+1));
        }
        vecs.emplace_back(std::move(next));
    };
    while(vecs.size() > 1) {
        int v = vecs.back().back();
        vecs.pop_back();
        vecs.back().push_back(vecs.back().back() - v);
    }
    return vecs.back().back();
}

int main() {
    std::string str_input = load_file_input("input.txt");
    std::vector<std::string_view> lines = split(str_input, '\n');
    int sum = 0;
    int sum2 = 0;
    for (auto sv : lines) {
        std::vector<std::string_view> nums_str = split(sv, ' ');
        std::vector<int> vals;
        std::transform(nums_str.begin(), nums_str.end(), std::back_inserter(vals), svtoi);
        int a = extrapolate(vals);
        int b = extrapolate_reverse(vals);
        std::cout << b << "..." <<  a << std::endl;
        sum += a;
        sum2 += b;
    }

    std::cout << "SUM: " << sum << std::endl;
    std::cout << "SUM2: " << sum2 << std::endl;
    return 0;

}
