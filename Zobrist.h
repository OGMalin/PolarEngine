#pragma once

#include "ChessDefs.h"

// keyValues to be XOR'ed
// key[0-11][0-63] = Value for each pieces on each square
// key[12][0-15]   = Value for castle rights
// key[12][16]     = Value for black on move.
// key[12][17-24]  = Value for ep file

extern HASHKEY ZobristKey[13][64];

extern void InitZobristKeys();

