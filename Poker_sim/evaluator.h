#pragma once
#include <iostream>
#include <fstream>
// Number of entries in the TwoPlusTwo HandRanks lookup table.
#define TABLE_SIZE 32487834
// Declaration only (definition must live in exactly one .cpp file).
extern int HR[TABLE_SIZE];

void load_handranks();
int evaluate7(int* cards);
