#include "Zobrist.h"
#include "Utility.h"

namespace
{
  static bool initZobrist=false;
}

HASHKEY ZobristKey[13][64];

void InitZobristKeys()
{
  if (initZobrist)
    return;
  int i,j;
  for (i=0;i<13;i++)
    for (j=0;j<64;j++)
      ZobristKey[i][j]=rand64();
  initZobrist=true;
}



