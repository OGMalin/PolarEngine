#pragma once

#include <cstring>
#include "ChessDefs.h"

class BasicBoard
{
public:
  typePiece  _board[0x88]; // 
  typeCastle castle;      // 
  typeSquare enPassant;   // Square to where the pawn arrive. e.g. a3,b3... a6,b6 etc.
  typeColor  toMove;      // BLACK or WHITE
  int  move50draw;
  BasicBoard(){ clear(); };

  BasicBoard(const BasicBoard& b){ copy(b);};

  virtual ~BasicBoard(){};

  // Overloading = so you can use newboard=oldboard;
  BasicBoard& operator=(const BasicBoard& b)
  {
    copy(b);
    return *this;
  };

  // Comparing two boards
  // returns: 0 - equal
  //          1 - pieces are different
  //          2 - The castle rights are different
  //          3 - The enpassant square is different
  //          4 - The color to move is different.
  virtual int compare(const BasicBoard& b)
  {
    if (memcmp(&_board,&(b._board),0x88*sizeof(typePiece))!=0) return 1;
    if (castle!=b.castle) return 2;
    if (enPassant!=b.enPassant) return 3;
    if (toMove!=b.toMove) return 4;
    return 0;
  }

  // Copies another board to this board
  virtual void copy(const BasicBoard& b)
  {
    memcpy(&_board, &b._board,0x88*sizeof(typePiece));
    castle=b.castle;
    enPassant=b.enPassant;
    toMove=b.toMove;
    move50draw=b.move50draw;
  };

  // Clearing the board
  virtual void clear()
  {
    typeSquare sq;
    for (sq=0;sq<0x88;sq++)
      _board[sq]=EMPTY;
    castle=0;
    enPassant=UNDEF; 
    toMove=WHITE;
    move50draw=0;
  };

  // Returning a piece. 
  // sq is in square in a 0x88 board
  typePiece getPiece(typeSquare sq){ return _board[sq]; };

  // Adding a piece
  // sq is in square in a 0x88 board
  virtual void setPiece(typeSquare sq, typePiece piece){ _board[sq]=piece;};

  bool whiteKingSideCastle(){ return (castle&whitekingsidecastle?true:false); };

  bool whiteQueenSideCastle(){ return (castle&whitequeensidecastle?true:false); };

  bool blackKingSideCastle(){ return (castle&blackkingsidecastle?true:false); };
  
  bool blackQueenSideCastle(){ return (castle&blackqueensidecastle?true:false);};
 
  void setCastleRights(typeCastle c){ castle=c; };

  typeSquare enPassantSquare(){ return enPassant; };

  void setEnPassantSquare(typeSquare ep){ enPassant=ep; };

  int get50MoveDraw(){ return move50draw; };

  void set50MoveDraw(int n){ move50draw=n; };

  bool whiteToMove(){ return (toMove==WHITE?true:false); };

  void setWhiteToMove(bool b){ toMove=b?WHITE:BLACK; };

  bool isEmpty()
  {
    typeSquare sq=0;
    while (1)
    {
      if (_board[sq]!=EMPTY)
        return false;
      sq++;
      if (sq&8)
      {
        sq+=8;
        if (sq>127)
          return true;
      }
    }
    return true;
  }
};

// This is a copy of BasicBoard without any constructors etc. because
// a union could not handle constructor, copy constructors etc.
struct msgBoard
{
  typePiece _board[0x88];
  typeCastle castle;    // 
  typeSquare enPassant;
  typeColor toMove;
  int move50draw;
  void set(const BasicBoard& b)
  {
    memcpy(&_board[0],&b._board[0],0x88*sizeof(typePiece));
    castle=b.castle;
    enPassant=b.enPassant;
    toMove=b.toMove;
    move50draw=b.move50draw;
  };
  void get(BasicBoard& b)
  {
    memcpy(&b._board[0],&_board[0],0x88*sizeof(typePiece));
    b.castle=castle;
    b.enPassant=enPassant;
    b.toMove=toMove;
    b.move50draw=move50draw;
  };
};

