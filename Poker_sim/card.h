#pragma once
#include <iostream>
// enum for the rank of the card
enum Rank {
    TWO = 0,
    THREE = 1,
    FOUR = 2,
    FIVE = 3,
    SIX = 4,
    SEVEN = 5,
    EIGHT = 6,
    NINE = 7,
    TEN = 8,
    JACK = 9,
    QUEEN = 10,
    KING = 11,
    ACE = 12,
};

// enum for the suit of the card
enum Suit {
    CLUBS = 0,
    DIAMONDS = 1,
    HEARTS = 2,
    SPADES = 3,
    SUIT_COUNT
};

//communicate between evaluator and reable format of the card
int cardToInt(Rank rank, Suit suit);
Rank parseRank(const std::string& rank);
Suit parseSuit(const std::string& suit);
int parseCard(const std::string& card);
std::string cardToString(int card);
