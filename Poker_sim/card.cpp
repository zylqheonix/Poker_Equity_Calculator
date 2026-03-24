#include "card.h"

int cardToInt(Rank rank, Suit suit) {
    int id = rank * 4 + suit + 1;
    return id;
}

Rank parseRank(const std::string& rank) {

    if(rank == "2") return TWO;
    if(rank == "3") return THREE;
    if(rank == "4") return FOUR;
    if(rank == "5") return FIVE;
    if(rank == "6") return SIX;
    if(rank == "7") return SEVEN;
    if(rank == "8") return EIGHT;
    if(rank == "9") return NINE;
    if(rank == "T") return TEN;
    if(rank == "J") return JACK;
    if(rank == "Q") return QUEEN;
    if(rank == "K") return KING;
    if(rank == "A") return ACE;
    throw std::invalid_argument("Invalid rank: " + rank);
};

Suit parseSuit(const::std::string& suit) {
    if(suit == "c") return CLUBS;
    if(suit == "d") return DIAMONDS;
    if(suit == "h") return HEARTS;
    if(suit == "s") return SPADES;
    throw std::invalid_argument("Invalid suit: " + suit);
};

int parseCard(const::std::string& card) {
    if(card.length() != 2) throw std::invalid_argument("Invalid card: " + card);
    Rank rank =  parseRank(card.substr(0, 1));
    Suit suit = parseSuit(card.substr(1, 1));
    return cardToInt(rank, suit);
};

std::string cardToString(int card) {
    int zeroBased = card - 1;
    Rank rank = (Rank)(zeroBased / 4);
    Suit suit = (Suit)(zeroBased % 4);
    return std::string(1, '2' + rank) + std::string(1, 'c' + suit);
};