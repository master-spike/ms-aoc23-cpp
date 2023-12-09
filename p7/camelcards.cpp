#include <string>
#include <vector>
#include <string_view>
#include <istream>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <set>

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

struct card {
    static constexpr std::array<char, 13> order{'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2'};
    
    char m_card;
    bool operator==(card const& other) const {
        return m_card == other.m_card;
    }
    bool operator<(card const& other) const {
        if (m_card == other.m_card) return false;
        for (auto c : order) {
            if (m_card == c) return true;
            if (other.m_card == c) return false;
        }
        return false;
    }
    card(char c) : m_card(c) {}
    card() = default;
};

namespace std {
    template<>
    struct hash<card> {
        std::size_t operator()(card const& c) const noexcept {
            return hash<char>{}(c.m_card);
        }
    };
}

enum class hand_category : int {
    FIVE_OF_A_KIND = 0,
    FOUR_OF_A_KIND = 1,
    FULL_HOUSE = 2,
    THREE_OF_A_KIND = 3,
    TWO_PAIR = 4,
    ONE_PAIR = 5,
    HIGH_CARD = 6
};

struct hand {
    std::array<card, 5> cards;
    hand_category category() const {
        std::unordered_map<card, int> counts;
        for (card const& c : cards) {
            if (counts.find(c) == counts.end()) {
                counts.emplace(c, std::count(cards.begin(), cards.end(), c));
            }
        }
        std::array<int, 5> counts_num{0};
        {
            auto it = counts_num.begin();
            for (auto jt = counts.begin(); jt != counts.end(); ++jt, ++it) {
                *it = jt->second;
            }
        }
        std::sort(counts_num.begin(), counts_num.end(), std::greater<int>());
        if (counts_num[0] == 5) return hand_category::FIVE_OF_A_KIND;
        if (counts_num[0] == 4) return hand_category::FOUR_OF_A_KIND;
        if (counts_num[0] == 3) {
            if (counts_num[1] == 2) return hand_category::FULL_HOUSE;
            else return hand_category::THREE_OF_A_KIND;
        }
        if (counts_num[0] == 2) {
            if (counts_num[1] == 2) return hand_category::TWO_PAIR;
            else return hand_category::ONE_PAIR;
        }
        return hand_category::HIGH_CARD;
    }
    bool operator==(hand const& other) const {
        return cards == other.cards;
    }
    bool operator<(hand const& other) const {
        //return (category() < other.category());
        if (category() == other.category()) {
            for (int i = 0; i < 5; ++i) {
                if (cards[i] < other.cards[i]) return true;
                if (other.cards[i] < cards[i]) return false;
            }
            return false;
        }
        else return (category() < other.category());
    }
    
    hand(std::string_view view) {
        for (std::size_t i = 0; i < std::min(std::size_t{5}, view.length()); ++i) {
            cards[i] = card(view[i]);
        }
    }
    
};

int main(int argc, char** argv) {
    auto parse_line = [](std::string_view line) -> std::pair<hand, int> {
        std::vector<std::string_view> parts = split(line, ' ');
        return std::make_pair(hand(parts[0]), svtoi(parts[1]));    
    };
    std::string input_str = load_file_input("input.txt");
    std::vector<std::pair<hand, int>> hands_to_bid;
    std::cout << input_str << std::endl;
    for (auto sv : split(input_str, '\n')) {
        hands_to_bid.emplace_back(parse_line(sv));
    }
    std::sort(hands_to_bid.begin(), hands_to_bid.end(), [](auto const& lhs, auto const& rhs){
        return lhs.first < rhs.first;
    });
    int current_score = hands_to_bid.size();
    int total_score = 0;
    for (auto const& hb : hands_to_bid) {
        std::cout << current_score << ": ";
        for (card c : hb.first.cards) {
            std::cout << c.m_card;
        }
        std::cout << " bid=" << hb.second << std::endl;
        total_score += current_score * hb.second;
        --current_score;
    }
    std::cout << total_score << std::endl;
    auto joker_hand_category = [](const hand& h) -> hand_category {
        int num_jokers = 0;
        std::array<int, 5> counts{0};
        int c_idx = 0;
        std::set<char> seen;
        for (card c : h.cards) {
            if (c.m_card == 'J') ++num_jokers;
            else if (seen.find(c.m_card) == seen.end()) {
                counts[c_idx] = std::count(h.cards.begin(), h.cards.end(), c);
                seen.emplace(c.m_card);
                ++c_idx;
            }
        }
        std::sort(counts.begin(), counts.end(), std::greater<int>());
        counts[0] += num_jokers;
        if (counts[0] == 5) return hand_category::FIVE_OF_A_KIND;
        if (counts[0] == 4) return hand_category::FOUR_OF_A_KIND;
        if (counts[0] == 3) {
            if (counts[1] == 2) return hand_category::FULL_HOUSE;
            else return hand_category::THREE_OF_A_KIND;
        }
        if (counts[0] == 2) {
            if (counts[1] == 2) return hand_category::TWO_PAIR;
            else return hand_category::ONE_PAIR;
        }
        return hand_category::HIGH_CARD;
    };

    auto lex_order = [](const hand& lhs, const hand& rhs) -> bool {
        for (int i = 0; i < 5; ++i) {
            if (lhs.cards[i] == rhs.cards[i]) continue;
            if (rhs.cards[i] == 'J') return true;
            if (lhs.cards[i] == 'J') return false;
            if (lhs.cards[i] < rhs.cards[i]) return true;
            if (rhs.cards[i] < lhs.cards[i]) return false;
        }
        return false;
    };
    auto cmp_hand_2 = [joker_hand_category, lex_order](const hand& lhs, const hand& rhs) -> bool {
        hand_category hcl = joker_hand_category(lhs);
        hand_category hcr = joker_hand_category(rhs);
        if (hcl > hcr) return false;
        if (hcl < hcr) return true;
        return lex_order(lhs, rhs);
    };
    std::sort(hands_to_bid.begin(), hands_to_bid.end(), [cmp_hand_2](auto const& lhs, auto const& rhs){
        return cmp_hand_2(lhs.first, rhs.first);
    });
    current_score = hands_to_bid.size();
    total_score = 0;
    for (auto const& hb : hands_to_bid) {
        total_score += hb.second * current_score;
        current_score--;
    }
    std::cout << total_score << std::endl;
    return 0;
}

