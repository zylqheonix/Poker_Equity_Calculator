#include "evaluator.h"
#include <stdexcept>
#include <string>

int HR[TABLE_SIZE];


void load_handranks() {
    std::ifstream file("HandRanks.dat", std::ios::binary);
    if (!file.is_open()) {
        file.open("Poker_sim/HandRanks.dat", std::ios::binary);
    }
    if (!file.is_open()) {
        throw std::runtime_error("Could not open HandRanks.dat");
    }
    file.read(reinterpret_cast<char*>(HR), sizeof(HR));
    if (!file) {
        throw std::runtime_error("Failed to read full HandRanks.dat");
    }
}

int evaluate7(int* cards) {
    int c[7] = {cards[0], cards[1], cards[2], cards[3], cards[4], cards[5], cards[6]};

    auto lookup = [&](int idx, int step) -> int {
        if (idx < 0 || idx >= TABLE_SIZE) {
            throw std::runtime_error(
                "Hand evaluator index out of bounds at step " + std::to_string(step) +
                " idx=" + std::to_string(idx) +
                " cards=[" +
                std::to_string(c[0]) + "," + std::to_string(c[1]) + "," + std::to_string(c[2]) + "," +
                std::to_string(c[3]) + "," + std::to_string(c[4]) + "," + std::to_string(c[5]) + "," +
                std::to_string(c[6]) + "]"
            );
        }
        return HR[idx];
    };

    int p = lookup(53 + *cards++, 1);
    p = lookup(p + *cards++, 2);
    p = lookup(p + *cards++, 3);
    p = lookup(p + *cards++, 4);
    p = lookup(p + *cards++, 5);
    p = lookup(p + *cards++, 6);
    return lookup(p + *cards++, 7);
}
