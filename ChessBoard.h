#pragma once

#include <string>

#include "BasicBoard.h"
#include "ChessMove.h"

enum {FIDE,SAN,LAN,COOR};

// This class adds some new function to the BasicBoard. 
class ChessBoard : public BasicBoard
{
public:
  ChessBoard();
  ChessBoard(BasicBoard& bb);
  virtual ~ChessBoard();

  // Do a move on the chessboard
  bool move(const char* sz);
  bool move(ChessMove& m, bool legalcheck=true);
  void takeback(ChessMove& m);

  // Possible moves
  int movecount();

  // Using a FEN string to setup the board
  void setFen(const char* szFen);
  
  // Get the fen-string for current position
  char* getFen(char* buffer);
  std::string getFen();

  // Strip 'non' important characters from a movestring
  char* stripMoveText(char* mt);

  // Convert a textmove to a move object
  const ChessMove getMoveFromText(const std::string text);

  // Find a move that would result in a position
  const ChessMove findMove(const BasicBoard& bb);

  bool isRowChar(char c);
  bool isFileChar(char c);
  bool isPieceChar(char c);  
  typePiece getPieceFromChar(char c);
  char getCharFromPiece(typePiece p);

  // Type:
  //  FIDE - Fide standard
  //  SAN  - PGN standard
  //  LAN  - Long algebraic Notation
  //  COOR - Coordinate system (Winboard standard)
  char* makeMoveText(const ChessMove& cm, char* buf, int bufsize, int type);

  void fixMove(ChessMove& m);

  bool isLegal();

	// Return:
	// 0 - No
	// 1 - Mate
	// 2 - Stalemate
  int gameEnd();

  bool insufficientMaterial();

  HASHKEY getHashkey();
};

