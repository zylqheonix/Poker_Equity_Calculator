#include "simulator.h"

double streetSimulation(
    int numPlayers,
    std::pair<int,int> heroCards,
    std::vector<int> boardCards,
    std::vector<std::string> opponentRange,
    int simulations,
    uint64_t seed
) {
    
    std::mt19937 rng;

    if(seed == 0)rng.seed(std::random_device{}() );
    else rng.seed(static_cast<std::mt19937::result_type>(seed));
    
    
    
    std::unordered_set<int> deadCards = {heroCards.first, heroCards.second};
    deadCards.insert(boardCards.begin(), boardCards.end());

    double heroEquitySum = 0.0;
    int completedIterations = 0;
    std::array<int, 7> heroHands;
    std::array<int, 7> villainHands;
    
    if(opponentRange.size() == 0) {

        const std::unordered_set<int> initialDeadCards = deadCards;
        const std::vector<int> initialBoard = boardCards;
        
        for(int iter = 0; iter < simulations; iter++) {

            deadCards = initialDeadCards;
            boardCards = initialBoard;

            std::vector<std::pair<int, int>> villianHoles;
            std::vector<int> remainingDeck;
            
            for(int card = 1; card <= 52; card++) {
                if(!deadCards.contains(card)) {
                    remainingDeck.push_back(card);
                }
            }

            std::shuffle(remainingDeck.begin(), remainingDeck.end(), rng);
            
            for(int v = 0; v < numPlayers; v++) {

                std::pair villianHand = {remainingDeck[0], remainingDeck[1]};
                villianHoles.push_back(villianHand);
                deadCards.insert(villianHand.first);
                deadCards.insert(villianHand.second);
                remainingDeck.erase(remainingDeck.begin());
                remainingDeck.erase(remainingDeck.begin());
            }
            
            const int cardsToDraw = 5 - static_cast<int>(boardCards.size());
            for(int k = 0; k < cardsToDraw; k++) {
                boardCards.push_back(remainingDeck[k]);
            }

            heroHands = {
                heroCards.first,
                heroCards.second,
                boardCards[0],
                boardCards[1],
                boardCards[2],
                boardCards[3],
                boardCards[4]
            };

            int heroScore = evaluate7(heroHands.data());
            int bestVillainScore = 0;
            int villainsAtBestScore = 0;

            for(int v = 0; v < numPlayers; v++) {
                villainHands = {
                    villianHoles[v].first,
                    villianHoles[v].second,
                    boardCards[0],
                    boardCards[1],
                    boardCards[2],
                    boardCards[3],
                    boardCards[4]
                };
                const int score = evaluate7(villainHands.data());
                if(v == 0 || score > bestVillainScore) {
                    bestVillainScore = score;
                    villainsAtBestScore = 1;
                } else if(score == bestVillainScore) {
                    villainsAtBestScore++;
                }
            }

            completedIterations++;
            if(heroScore > bestVillainScore) {
                heroEquitySum += 1.0;
            } else if(heroScore == bestVillainScore) {
                heroEquitySum += 1.0 / static_cast<double>(villainsAtBestScore + 1);
            }
        }
        if (completedIterations == 0) return 0.0;
        return heroEquitySum / static_cast<double>(completedIterations);
    }

    else {
        
        const std::vector<std::pair<int, int>> initialCombos = getCombos(deadCards, opponentRange);

        if (initialCombos.empty()) {
            return 0.0;
        }

        const std::unordered_set<int> initialDeadCards = deadCards;
        const std::vector<int> initialBoard = boardCards;

        for(int iter = 0; iter < simulations; iter++) {

            deadCards = initialDeadCards;
            boardCards = initialBoard;
            std::vector<std::pair<int, int>> opponentCombos = initialCombos;
            std::vector<std::pair<int, int>> villianHoles;
            bool validIteration = true;
            std::uniform_int_distribution<int> dist(0, static_cast<int>(opponentCombos.size()) - 1);

            for(int v = 0; v < numPlayers; v++) {
                if(opponentCombos.empty()) {
                    validIteration = false;
                    break;
                }
                int randomIndex = dist(rng);

                std::pair<int, int> villianHand = opponentCombos[randomIndex];
                villianHoles.push_back(villianHand);

                deadCards.insert(villianHand.first);
                deadCards.insert(villianHand.second);

                opponentCombos.erase(
                    std::remove_if(opponentCombos.begin(), opponentCombos.end(),
                        [&](const std::pair<int, int>& combo) {
                            return deadCards.contains(combo.first) ||
                                   deadCards.contains(combo.second);
                        }),
                    opponentCombos.end()
                );

                if(!opponentCombos.empty()) {
                    dist = std::uniform_int_distribution<int>(0, static_cast<int>(opponentCombos.size()) - 1);
                }
            }
            if(!validIteration) {
                continue;
            }

            std::vector<int> remainingDeck;
            for(int card = 1; card <= 52; card++) {
                if(!deadCards.contains(card)) {
                    remainingDeck.push_back(card);
                }
            }

            std::shuffle(remainingDeck.begin(), remainingDeck.end(), rng);

            const int cardsToDraw = 5 - static_cast<int>(boardCards.size());
            for(int k = 0; k < cardsToDraw; k++) {
                boardCards.push_back(remainingDeck[k]);
            }

            heroHands = {
                heroCards.first,
                heroCards.second,
                boardCards[0],
                boardCards[1],
                boardCards[2],
                boardCards[3],
                boardCards[4]
            };
            int heroScore = evaluate7(heroHands.data());
            int bestVillainScore = 0;
            int villainsAtBestScore = 0;
            for(int v = 0; v < numPlayers; v++) {
                villainHands = {
                    villianHoles[v].first,
                    villianHoles[v].second,
                    boardCards[0],
                    boardCards[1],
                    boardCards[2],
                    boardCards[3],
                    boardCards[4]
                };
                const int score = evaluate7(villainHands.data());
                if(v == 0 || score > bestVillainScore) {
                    bestVillainScore = score;
                    villainsAtBestScore = 1;
                } else if(score == bestVillainScore) {
                    villainsAtBestScore++;
                }
            }

            completedIterations++;
            if(heroScore > bestVillainScore) {
                heroEquitySum += 1.0;
            } else if(heroScore == bestVillainScore) {
                heroEquitySum += 1.0 / static_cast<double>(villainsAtBestScore + 1);
            }
        }

        if (completedIterations == 0) return 0.0;
        return heroEquitySum / static_cast<double>(completedIterations);
    }
}

double PreFlopSimulation(int numPlayers,std::pair<int,int> heroCards, std::vector<std::string> opponentRanges, int simulations, uint64_t seed) {
    return streetSimulation(numPlayers, heroCards, {}, opponentRanges, simulations, seed);
}

double FlopSimulation(int numPlayers,std::pair<int,int> heroCards, std::vector<int> boardCards, std::vector<std::string> opponentRanges, int simulations, uint64_t seed) {
    return streetSimulation(numPlayers, heroCards, boardCards, opponentRanges, simulations, seed);
}

double TurnSimulation(int numPlayers,std::pair<int,int> heroCards, std::vector<int> boardCards, std::vector<std::string> opponentRanges, int simulations, uint64_t seed) {
    return streetSimulation(numPlayers, heroCards, boardCards, opponentRanges, simulations, seed);
}

double RiverSimulation(int numPlayers,std::pair<int,int> heroCards, std::vector<int> boardCards, std::vector<std::string> opponentRanges, int simulations, uint64_t seed) {
    return streetSimulation(numPlayers, heroCards, boardCards, opponentRanges, simulations, seed);
}