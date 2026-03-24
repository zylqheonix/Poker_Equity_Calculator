#pragma once
#include <iostream>
#include "evaluator.h"
#include "card.h"
#include "range.h"
#include <vector>
#include <random>
#include <algorithm>

double streetSimulation(int numPlayers, std::pair<int,int> heroCards, std::vector<int> boardCards, 
    std::vector<std::string> opponentRange, uint64_t seed = 0);

double PreFlopSimulation(int numPlayers, std::pair<int,int> heroCards, std::vector<std::string> opponentRanges, uint64_t seed = 0);

double FlopSimulation(int numPlayers,std::pair<int,int> heroCards, std::vector<int> boardCards, std::vector<std::string> opponentRanges, uint64_t seed = 0);

double TurnSimulation(int numPlayers,std::pair<int,int> heroCards, std::vector<int> boardCards, std::vector<std::string> opponentRanges, uint64_t seed = 0);

double RiverSimulation(int numPlayers,std::pair<int,int> heroCards, std::vector<int> boardCards, std::vector<std::string> opponentRanges, uint64_t seed = 0);                                                                                       