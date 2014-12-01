#pragma once

#include "Zobrist.h"
#include "ChessBoard.h"

// This table is for use when exact draw claim is expected.
// It will use a hash key plus the real position. The first
// test is against the hash key and if this is true it would
// test against the real position before it declaire the
// position as equal.

// Entries in the drawtable
struct DrawTableEntry
{
  HASHKEY key;     // Hash key to the position.
  BasicBoard board; // Comprimized position.
};

class DrawTable
{
  DrawTableEntry table[1000];  // The table.
  int size;        // The size of the table (pointer to next element).
public:
  // Construct an empty table.
  DrawTable(){clear();};

  // Add a position to the table.
  void add(ChessBoard& b)
  {
    if (size>=1000)
      return;
    table[size].key=b.getHashkey();
		table[size].board = b;
    size++;
  };


  // Remove last element.
  void remove()
  {
    if (size)
      size--;
  }

  // Clear the drawtable
  void clear(){size=0;};

  // A check for existence of a position\. The first function would generate
  // a hashkey by itself so the caller don't need to to this if it's not
  // already generated.
  bool exist(ChessBoard& b)
  {
		HASHKEY hk = b.getHashkey();
    return exist(b,hk);
  };

  bool exist(ChessBoard& b,HASHKEY& hk)
  {
    int it=0;
    while (it<size)
    {
      if (table[it].key==hk)
      {
        if (b.compare(table[it].board)==0)
          return true;
      }
      ++it;
    }
    return false;    
  };
};
