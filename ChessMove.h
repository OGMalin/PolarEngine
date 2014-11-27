#pragma once

#include "ChessDefs.h"
#include <memory.h>

class ChessMove
{
public:
  typeSquare fromSquare;
  typeSquare toSquare;
  unsigned char moveType;
  typePiece promotePiece;
  typePiece capturedpiece;
  typeCastle oldCastle;
  typeSquare oldEnPassant;
  int oldMove50draw;
  int score;

  // Constructor
  ChessMove(){ clear(); };
  
  // Copy constructor
  ChessMove(const ChessMove& m)
  {
    if (&m!=this)
      memcpy(this,&m,sizeof(ChessMove));
  };
  
  // Destructor
  virtual ~ChessMove(){};

  // Assignment
  ChessMove& operator=(const ChessMove& m)
  {
    if (&m!=this)
      memcpy(this,&m,sizeof(ChessMove));
    return *this;
  }

  // Clear the move.
  void clear()
  {
    memset(this,0,sizeof(ChessMove));
    oldEnPassant=UNDEF;
  };

  // Check if the move is empty
  bool empty()
  {
    if (moveType&NULL_MOVE)
      return false;
    return (fromSquare==toSquare?true:false);
  };

  // Overload of ==
  friend bool operator==(const ChessMove& m1, const ChessMove& m2)
  {
    if ((m1.moveType&NULL_MOVE) || (m2.moveType&NULL_MOVE))
    {
      if ((m1.moveType&NULL_MOVE) && (m2.moveType&NULL_MOVE))
        return true;
      return false;
    }
    if (m1.fromSquare==m2.fromSquare)
      if (m1.toSquare==m2.toSquare)
        if (m1.promotePiece==m2.promotePiece)
          return true;
    return false;
  };

  // Overload of !=
  friend bool operator!=(const ChessMove& m1, const ChessMove& m2)
  {
    if ((m1.moveType&NULL_MOVE) || (m2.moveType&NULL_MOVE))
    {
      if ((m1.moveType&NULL_MOVE) && (m2.moveType&NULL_MOVE))
        return false;
      return true;
    }


    if (m1.fromSquare!=m2.fromSquare)
      return true;
    if (m1.toSquare!=m2.toSquare)
      return true;
    if (m1.promotePiece!=m2.promotePiece)
       return true;
    return false;
  };
};

// A copy af ChessMove without constructors etc.
struct msgMove
{
  typeSquare fromSquare;
  typeSquare toSquare;
  unsigned char moveType;
  typePiece promotePiece;
  typePiece capturedpiece;
  typeCastle oldCastle;
  typeSquare oldEnPassant;
  int oldMove50draw;
  int score;
  void set(ChessMove& m)
  {
    fromSquare=m.fromSquare;
    toSquare=m.toSquare;
    moveType=m.moveType;
    promotePiece=m.promotePiece;
    capturedpiece=m.capturedpiece;
    oldCastle=m.oldCastle;
    oldEnPassant=m.oldEnPassant;
    oldMove50draw=m.oldMove50draw;
    score=m.score;
  };

  void get(ChessMove& m)
  {
    m.fromSquare=fromSquare;
    m.toSquare=toSquare;
    m.moveType=moveType;
    m.promotePiece=promotePiece;
    m.capturedpiece=capturedpiece;
    m.oldCastle=oldCastle;
    m.oldEnPassant=oldEnPassant;
    m.oldMove50draw=oldMove50draw;
    m.score=score;
  };
};


