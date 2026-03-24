#include "evaluator.h"
#include <stdexcept>

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
    int p = HR[53 + *cards++];
    p = HR[p + *cards++];
    p = HR[p + *cards++];
    p = HR[p + *cards++];
    p = HR[p + *cards++];
    p = HR[p + *cards++];
    return HR[p + *cards++];
}
