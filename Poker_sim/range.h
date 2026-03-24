#pragma once
#include <unordered_set>
#include <utility>
#include <vector>
#include "card.h"
#include <algorithm>
#include <stdexcept>

std::vector<std::pair<int, int>> getCombos(const std::unordered_set<int>& deadCards, const std::vector<std::string>& range_string);
