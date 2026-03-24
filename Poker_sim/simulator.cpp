#include "simulator.h"

double streetSimulation(int numPlayers,std::pair<int,int> heroCards, std::vector<int> boardCards, 
    std::vector<std::string> opponentRange, uint64_t seed) {
    
    std::mt19937 rng;

    if(seed == 0)rng.seed(std::random_device{}() );
    else rng.seed(static_cast<std::mt19937::result_type>(seed));
    
    
    
    std::unordered_set<int> deadCards = {heroCards.first, heroCards.second};
    deadCards.insert(boardCards.begin(), boardCards.end());

    int HeroWins = 0;
    int VillainWins = 0;
    std::array<int, 7> heroHands;
    std::array<int, 7> villainHands;
    
    if(opponentRange.size() == 0) {

        const std::unordered_set<int> initialDeadCards = deadCards;
        const std::vector<int> initialBoard = boardCards;
        
        for(int iter = 0; iter < 10000; iter++) {

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
            
            for(int k = 0; k < 5 - static_cast<int>(boardCards.size()); k++) {
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
            int villainScore = 0;
           
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
                villainScore = std::max(villainScore, evaluate7(villainHands.data()));
            }

            
            if(heroScore > villainScore) {
                HeroWins++;
            } else if(villainScore > heroScore) {
                VillainWins++;
            }
        }
        return static_cast<double>(HeroWins) / static_cast<double>(HeroWins + VillainWins);
    }

    else {
        
        const std::vector<std::pair<int, int>> initialCombos = getCombos(deadCards, opponentRange);

        if (initialCombos.empty()) {
            return 0.0;
        }

        const std::unordered_set<int> initialDeadCards = deadCards;
        const std::vector<int> initialBoard = boardCards;

        for(int iter = 0; iter < 10000; iter++) {

            deadCards = initialDeadCards;
            boardCards = initialBoard;
            std::vector<std::pair<int, int>> opponentCombos = initialCombos;
            std::vector<std::pair<int, int>> villianHoles;
            std::uniform_int_distribution<int> dist(0, static_cast<int>(opponentCombos.size()) - 1);

            for(int v = 0; v < numPlayers; v++) {
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

            std::vector<int> remainingDeck;
            for(int card = 1; card <= 52; card++) {
                if(!deadCards.contains(card)) {
                    remainingDeck.push_back(card);
                }
            }

            std::shuffle(remainingDeck.begin(), remainingDeck.end(), rng);

            for(int k = 0; k < 5 - static_cast<int>(boardCards.size()); k++) {
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
            int villainScore = 0;
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
                villainScore = std::max(villainScore, evaluate7(villainHands.data()));
            }

            if(heroScore > villainScore) {
                HeroWins++;
            } else if(villainScore > heroScore) {
                VillainWins++;
            }
        }

        return static_cast<double>(HeroWins) / static_cast<double>(HeroWins + VillainWins);
    }
}

double PreFlopSimulation(int numPlayers,std::pair<int,int> heroCards, std::vector<std::string> opponentRanges, uint64_t seed) {
    return streetSimulation(numPlayers, heroCards, {}, opponentRanges, seed);
}

double FlopSimulation(int numPlayers,std::pair<int,int> heroCards, std::vector<int> boardCards, std::vector<std::string> opponentRanges, uint64_t seed) {
    return streetSimulation(numPlayers, heroCards, boardCards, opponentRanges, seed);
}

double TurnSimulation(int numPlayers,std::pair<int,int> heroCards, std::vector<int> boardCards, std::vector<std::string> opponentRanges, uint64_t seed) {
    return streetSimulation(numPlayers, heroCards, boardCards, opponentRanges, seed);
}

double RiverSimulation(int numPlayers,std::pair<int,int> heroCards, std::vector<int> boardCards, std::vector<std::string> opponentRanges, uint64_t seed) {
    return streetSimulation(numPlayers, heroCards, boardCards, opponentRanges, seed);
}