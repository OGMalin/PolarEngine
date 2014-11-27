#pragma once

#include "BasicBoard.h"
#include "ChessMove.h"
#include "MoveList.h"
#include "Relations.h"

// There is two method to find the knight square
// Both makeKnightMoves and makeNoSlideMoves. Therefore
// this two array of knight moves. One will be removed when
// I have tested speed/size.
static typeSquare knightMoves[0x88][8];

class MoveGenerator // : public BasicBoard
{
protected:
  ChessMove testMove;
  virtual void addPawnMoves(BasicBoard& b, MoveList& ml, typeSquare sq);
  virtual void addPawnPromote(BasicBoard& b, MoveList& ml, ChessMove& m);
  virtual void addNoSlideMoves(BasicBoard& b, MoveList& ml, typeSquare sq, const int* sqadd);
  virtual void addSlideMoves(BasicBoard& b, MoveList& ml, typeSquare sq, const int* sqadd);
  virtual void addKingMoves(BasicBoard& b, MoveList& ml, typeSquare sq);
  virtual void addKnightMoves(BasicBoard& b, MoveList& ml, typeSquare sq);
  virtual void addPawnCaptureMoves(BasicBoard& b, MoveList& ml, typeSquare sq);
  virtual void addNoSlideCaptureMoves(BasicBoard& b, MoveList& ml, typeSquare sq, const int* sqadd);
  virtual void addSlideCaptureMoves(BasicBoard& b, MoveList& ml, typeSquare sq, const int* sqadd);
  virtual void addKnightCaptureMoves(BasicBoard& b, MoveList& ml, typeSquare sq);
  // color is color to do the attack
  virtual bool squareAttacked(BasicBoard& b, typeSquare sq, typeColor color);
  // Initiate the movegeneration
  virtual void init();
public:
  MoveGenerator();
  virtual ~MoveGenerator();
  // color= color to test for
  virtual bool inCheck(BasicBoard& b, typeColor color);
  // Makes all moves without checking
  virtual void makeAllMoves(BasicBoard& b, MoveList& ml);
  // Makes all legal moves
  virtual void makeMoves(BasicBoard& b, MoveList& ml);
  // Make all capture and promote moves without checking
  virtual void makeAllCaptureMoves(BasicBoard& b, MoveList& ml);
  // Make all legal capture and promote moves
  virtual void makeCaptureMoves(BasicBoard& b, MoveList& ml);
  // Do/undo a move without checking
  virtual void doMove(BasicBoard& b, ChessMove& m);
  virtual void undoMove(BasicBoard& b, ChessMove& m);
  virtual void doNullMove(BasicBoard& b, ChessMove& m);
  virtual void undoNullMove(BasicBoard& b, ChessMove& m);
  // Do/undo a move with checking for legality
  virtual bool isLegal(BasicBoard&, ChessMove& m);
  virtual bool doLegalMove(BasicBoard&, ChessMove& m);
};

