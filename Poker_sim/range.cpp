#include "range.h"


std::vector<std::pair<int, int>> getCombos(
    const std::unordered_set<int>& deadCards,
    const std::vector<std::string>& range_string
) {
    std::vector<std::pair<int, int>> combos;

    for (const std::string& pair : range_string) {
        if (pair.length() == 2 && pair[0] == pair[1]) {
            for (int i = 0; i < SUIT_COUNT; ++i) {
                for (int j = i + 1; j < SUIT_COUNT; ++j) {
                    combos.push_back(std::make_pair(
                        cardToInt(parseRank(std::string(1, pair[0])), static_cast<Suit>(i)),
                        cardToInt(parseRank(std::string(1, pair[1])), static_cast<Suit>(j))
                    ));
                }
            }
        } else if (pair.length() == 3 && pair[2] == 's') {
            // Suited: XYs -> 4 combos
            for (int i = 0; i < SUIT_COUNT; ++i) {
                combos.push_back(std::make_pair(
                    cardToInt(parseRank(std::string(1, pair[0])), static_cast<Suit>(i)),
                    cardToInt(parseRank(std::string(1, pair[1])), static_cast<Suit>(i))
                ));
            }
        } else if (pair.length() == 3 && pair[2] == 'o') {
            for (int i = 0; i < SUIT_COUNT; ++i) {
                for (int j = 0; j < SUIT_COUNT; ++j) {
                    if (i == j) {
                        continue;
                    }
                    combos.push_back(std::make_pair(
                        cardToInt(parseRank(std::string(1, pair[0])), static_cast<Suit>(i)),
                        cardToInt(parseRank(std::string(1, pair[1])), static_cast<Suit>(j))
                    ));
                }
            }
        } else {
            throw std::invalid_argument("Invalid range string: " + pair);
        }
    }

    combos.erase(
        std::remove_if(combos.begin(), combos.end(),
            [&](const std::pair<int, int>& combo) {
                return deadCards.contains(combo.first) ||
                       deadCards.contains(combo.second);
            }),
        combos.end()
    );

    return combos;
}
