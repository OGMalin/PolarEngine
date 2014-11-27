#include "MoveGenerator.h"
#include <cstdlib>

#ifdef _DEBUG
#include <iostream>
using namespace std;
#endif

#ifdef _DEBUG_MOVEGEN
#include <iostream>
using namespace std;
#endif

static bool initMoveGen=0;

MoveGenerator::MoveGenerator() 
{
  if (!initMoveGen)
    init();
};

MoveGenerator::~MoveGenerator()
{
};

void MoveGenerator::init()
{
  int i,j;
  typeSquare sq;
  // Generate all knightmoves;
  testMove.clear();
  for (sq=0;sq<128;sq++)
    for (i=0;i<8;i++)
      knightMoves[sq][i]=UNDEF;
  sq=0;
  while(1)
  {
    j=0;
    for (i=0;i<8;i++)
    {
      if (LEGALSQUARE(sq+knightPath[i]))
        knightMoves[sq][j++]=sq+knightPath[i];
    }
    sq++;
    if (sq&8)
    {
      sq+=8;
      if (sq>127)
        break;
    }
  }
}

void MoveGenerator::makeAllCaptureMoves(BasicBoard& b, MoveList& ml)
{
  typeSquare sq=0;
  ml.clear();
  typeColor player=b.toMove;
  // Add all possible capture moves without testing for check
  while(1)
  {
    typePiece piece=b._board[sq];
    if (PIECECOLOR(piece)==player)
    {
      switch (PIECE(piece))
      {
        case PAWN:
          addPawnCaptureMoves(b,ml,sq);
          break;
        case KNIGHT:
//          addNoSlideCaptureMoves(b,ml,sq,knightPath);
          addKnightCaptureMoves(b,ml,sq);
          break;
        case BISHOP:
          addSlideCaptureMoves(b,ml,sq,bishopPath);
          break;
        case ROOK:
          addSlideCaptureMoves(b,ml,sq,rookPath);
          break;
        case QUEEN:
          addSlideCaptureMoves(b,ml,sq,bishopPath);
          addSlideCaptureMoves(b,ml,sq,rookPath);
          break;
        case KING:
          addNoSlideCaptureMoves(b,ml,sq,kingPath);
          break;
      }
    }
    sq++;
    if (sq&8)
    {
      sq+=8;
      if (sq>127)
        break;
    }
  }
}

void MoveGenerator::makeCaptureMoves(BasicBoard& b, MoveList& ml)
{

  typeColor player=b.toMove;
  int other=OTHERPLAYER(player);
  #ifdef _DEBUG_MOVEGEN
    BasicBoard dbgB;
    dbgB.copy(b);
  #endif
  makeAllCaptureMoves(b,ml);
 
  #ifdef _DEBUG_MOVEGEN
    if (b.compare(dbgB))
      cerr << "**error** MoveGen\n";
  #endif

  typePiece p=COLORPIECE(player,KING);
  typeSquare sq=0,nsq;
  while(1)
  {
    if (b._board[sq]==p)
      break;
    sq++;
    if (sq&8)
    {
      sq+=8;
      if (sq>127)
        return;
    }
  }

  int it=0;
  
  while (it<ml.size)
  {
    if (b._board[ml.list[it].fromSquare]==p)
      nsq=ml.list[it].toSquare;
    else
      nsq=sq;
    doMove(b,ml.list[it]);
    if (squareAttacked(b,nsq,other))
    {
      undoMove(b,ml.list[it]);
      #ifdef _DEBUG_MOVEGEN
        if (b.compare(dbgB))
          cerr << "**error** MoveGen\n";
      #endif
      ml.list[it].clear();
    }else
    {
      undoMove(b,ml.list[it]);
      #ifdef _DEBUG_MOVEGEN
        if (b.compare(dbgB))
          cerr << "**error** MoveGen\n";
      #endif
    }
    ++it;
  };

  ml.trunc();
}

void MoveGenerator::makeAllMoves(BasicBoard& b, MoveList& ml)
{
  typeSquare sq=0;
  ml.clear();
  typeColor player=b.toMove;

  // Add all possible moves without testing for check
  while (1)
  {
    typePiece piece=b._board[sq];
    if (PIECECOLOR(piece)==player)
    {
      switch (PIECE(piece))
      {
        case PAWN:
          addPawnMoves(b,ml,sq);
          break;
        case KNIGHT:
//          addNoSlideMoves(b,ml,sq,knightPath);
          addKnightMoves(b,ml,sq);
          break;
        case BISHOP:
          addSlideMoves(b,ml,sq,bishopPath);
          break;
        case ROOK:
          addSlideMoves(b,ml,sq,rookPath);
          break;
        case QUEEN:
          addSlideMoves(b,ml,sq,bishopPath);
          addSlideMoves(b,ml,sq,rookPath);
          break;
        case KING:
          addKingMoves(b,ml,sq);
          break;
      }
    }
    sq++;
    if (sq&8)
    {
      sq+=8;
      if (sq>127)
        return;
    }
  }
}

void MoveGenerator::makeMoves(BasicBoard& b, MoveList& ml)
{
  typeColor player=b.toMove;
  typeColor other=OTHERPLAYER(player);

  // Make all moves (both legal and illegal)
  makeAllMoves(b,ml);

  // Find the king  
  typePiece p=COLORPIECE(player,KING);
  typeSquare sq=0,nsq;
  while (1)
  {
    if (b._board[sq]==p)
      break;
    sq++;
    if (sq&8)
    {
      sq+=8;
      if (sq>127)
        return;
    }
  }

  int it=0;
  while (it<ml.size)
  {
    if (ml.list[it].moveType&CASTLE)
    {
      if (squareAttacked(b, ml.list[it].fromSquare,other))
      {
        ml.list[it].clear();
        ++it;
        continue;
      }
      if (squareAttacked(b, __max(ml.list[it].fromSquare,ml.list[it].toSquare)-1,other))
      {
        ml.list[it].clear();
        ++it;
        continue;
      }
    }
    if (b._board[ml.list[it].fromSquare]==p)
      nsq=ml.list[it].toSquare;
    else
      nsq=sq;
    doMove(b,ml.list[it]);
    if (squareAttacked(b,nsq,other))
    {
      undoMove(b,ml.list[it]);
      ml.list[it].clear();
    }else
    {
      undoMove(b,ml.list[it]);
    }
    ++it;
  };

  ml.trunc();
  return ;
}

// Is a move legal
bool MoveGenerator::isLegal(BasicBoard& b, ChessMove& m)
{
  typeColor color=b.toMove;

  if (m.empty())
    return false;
  if (m.moveType&CASTLE)
  {
    if (squareAttacked(b, m.fromSquare,OTHERPLAYER(b.toMove)))
      return false;
    if (squareAttacked(b, __max(m.fromSquare,m.toSquare)-1,OTHERPLAYER(b.toMove)))
      return false;
  }
  doMove(b,m);
  if (inCheck(b,color))
  {
    undoMove(b,m);
    return false;
  }
  undoMove(b,m);
  return true;
}

bool MoveGenerator::doLegalMove(BasicBoard& b, ChessMove& m)
{
  typeColor color=b.toMove;

  if (m.moveType&CASTLE)
  {
    if (squareAttacked(b, m.fromSquare,OTHERPLAYER(b.toMove)))
      return false;
    if (squareAttacked(b, __max(m.fromSquare,m.toSquare)-1,OTHERPLAYER(b.toMove)))
      return false;
  }
  doMove(b,m);
  if (inCheck(b,color))
  {
    undoMove(b,m);
    return false;
  }
  return true;
}

// This function check if a square is attacked by a pieces.
// Instead of making all moves and see if one of the pieces hit this square
// I'm puting a piece on the square in see if it hit it self.
bool MoveGenerator::squareAttacked(BasicBoard& b, typeSquare sq, typeColor color)
{
  int i,testSquare;
  typePiece pawn=COLORPIECE(color,PAWN);
  typePiece knight=COLORPIECE(color,KNIGHT);
  typePiece king=COLORPIECE(color,KING);
  typePiece rook=COLORPIECE(color,ROOK);
  typePiece bishop=COLORPIECE(color,BISHOP);
  typePiece queen=COLORPIECE(color,QUEEN);

  // Attacked from diagonals
  for (i=0;i<4;i++)
  {
    testSquare=sq+bishopPath[i];
    while (LEGALSQUARE(testSquare))
    {
      if (b._board[testSquare]!=EMPTY)
      {
        if ((b._board[testSquare]==bishop) || (b._board[testSquare]==queen))
          return true;
        break;
      }
      testSquare+=bishopPath[i];
    }
  }

  // Attacked from rank and files
  for (i=0;i<4;i++)
  {
    testSquare=sq+rookPath[i];
    while (LEGALSQUARE(testSquare))
    {
      if (b._board[testSquare]!=EMPTY)
      {
        if ((b._board[testSquare]==rook) || (b._board[testSquare]==queen))
          return true;
        break;
      }
      testSquare+=rookPath[i];
    }
  }

  // Attacked from knights or king
  for (i=0;i<8;i++)
  {
    testSquare=sq+knightPath[i];
    if (LEGALSQUARE(testSquare))
      if (b._board[testSquare]==knight)
        return true;
    testSquare=sq+kingPath[i];
    if (LEGALSQUARE(testSquare))
      if (b._board[testSquare]==king)
        return true;
  }

  // Attacked from pawns
  testSquare=sq-1+(color?16:-16);
  for (i=0;i<2;i++)
  {
    if (LEGALSQUARE(testSquare))
      if (b._board[testSquare]==pawn)
        return true;
    testSquare+=2;
  }
  return false;
}

bool MoveGenerator::inCheck(BasicBoard& b, typeColor color)
{
  typePiece p=COLORPIECE(color,KING);
  typeSquare sq=0;
  while (1)
  {
    if (b._board[sq]==p)
      break;
    sq++;
    if (sq&8)
    {
      sq+=8;
      if (sq>127)
        return false;
    }
  }
  return squareAttacked(b,sq,OTHERPLAYER(color));
}

void MoveGenerator::addPawnMoves(BasicBoard& b, MoveList& ml, typeSquare sq)
{
  int pawnRow,i,capfile;
  pawnRow=(b.toMove)?-16:16;

  // One square forward;
  testMove.clear();
  testMove.fromSquare=sq;
	testMove.toSquare=testMove.fromSquare+pawnRow;
  if (b._board[testMove.toSquare]==EMPTY)
  {
    if ((testMove.toSquare<a2)||(testMove.toSquare>h7))
    {
      testMove.moveType=PAWNMOVE|PROMOTE;
      addPawnPromote(b,ml,testMove);
    }else
    {
      testMove.moveType=PAWNMOVE;
      ml.push_back(testMove);
    }
  }

  // Capture
  testMove.clear();
  testMove.fromSquare=sq;
  capfile=-1; // Look first to left
  for (i=0;i<2;i++)
  {
    testMove.toSquare=sq+capfile+pawnRow;
    if (LEGALSQUARE(testMove.toSquare))
    {
      if ((b._board[testMove.toSquare]!=EMPTY) && 
          (PIECECOLOR(b._board[testMove.toSquare])!=b.toMove))
			{
        testMove.capturedpiece=b._board[testMove.toSquare];
        if ((testMove.toSquare<a2)||(testMove.toSquare>h7))
        {
          testMove.moveType=PAWNMOVE|CAPTURE|PROMOTE;
          addPawnPromote(b,ml,testMove);
        }else
        {
          testMove.moveType=PAWNMOVE|CAPTURE;
          ml.push_back(testMove);
        }
      }else if (testMove.toSquare==b.enPassant)
      {
        testMove.moveType=PAWNMOVE|CAPTURE|ENPASSANT;
        if (b.toMove==WHITE)
          testMove.capturedpiece=blackpawn;
        else
          testMove.capturedpiece=whitepawn;
        ml.push_back(testMove);
      }
    }
	  capfile=1;  // Look to the right
  }

// Two square forward
  if (((RANK(sq)==1)&&(b.toMove==WHITE)) || 
      ((RANK(sq)==6)&&(b.toMove==BLACK)))
  {
    testMove.clear();
    testMove.fromSquare=sq;
    testMove.moveType=PAWNMOVE|DBLPAWNMOVE;
    if ((b._board[sq+pawnRow]==EMPTY) && 
        (b._board[sq+pawnRow*2]==EMPTY))
    {
  	  testMove.toSquare=testMove.fromSquare+pawnRow*2;
      ml.push_back(testMove);
    }
  }
}

void MoveGenerator::addPawnPromote(BasicBoard& b, MoveList& ml, ChessMove& m)
{
  m.promotePiece=COLORPIECE(b.toMove,QUEEN);
  ml.push_back(m);
  m.promotePiece=COLORPIECE(b.toMove,ROOK);
  ml.push_back(m);
  m.promotePiece=COLORPIECE(b.toMove,BISHOP);
  ml.push_back(m);
  m.promotePiece=COLORPIECE(b.toMove,KNIGHT);
  ml.push_back(m);
};

// Knight and king
void MoveGenerator::addNoSlideMoves(BasicBoard& b, MoveList& ml, typeSquare sq, const int* sqadd)
{
  int i;
  testMove.clear();
  testMove.fromSquare=sq;
  for (i=0;i<8;i++)
  {
    testMove.toSquare=sq+sqadd[i];
    if (LEGALSQUARE(testMove.toSquare))
    {
      if (b._board[testMove.toSquare]==EMPTY)
      {
        ml.push_back(testMove);
      }else if (PIECECOLOR(b._board[testMove.toSquare])!=b.toMove)
      {
        testMove.moveType=CAPTURE;
        testMove.capturedpiece=b._board[testMove.toSquare];
        ml.push_back(testMove);
        testMove.moveType=0;
        testMove.capturedpiece=0;
      }
    }
  }
}

// Queen, bishop and rook
void MoveGenerator::addSlideMoves(BasicBoard& b, MoveList& ml, typeSquare sq, const int* sqadd)
{
  int i;
  testMove.clear();
  testMove.fromSquare=sq;
  for (i=0;i<4;i++)
  {
    testMove.toSquare=sq+sqadd[i];
    while (LEGALSQUARE(testMove.toSquare))
    {
      if (b._board[testMove.toSquare]==EMPTY)
      {
        ml.push_back(testMove);
      }else if (PIECECOLOR(b._board[testMove.toSquare])!=b.toMove)
      {
        testMove.moveType=CAPTURE;
        testMove.capturedpiece=b._board[testMove.toSquare];
        ml.push_back(testMove);
        testMove.moveType=0;
        testMove.capturedpiece=0;
        break;
      }else
      {
        break;
      }
      testMove.toSquare+=sqadd[i];
    }
  }
}

void MoveGenerator::addKingMoves(BasicBoard& b, MoveList& ml, typeSquare sq)
{
  typeCastle ctl;
  addNoSlideMoves(b,ml,sq,kingPath);

  // castle
  testMove.clear();
  testMove.fromSquare=sq;
  testMove.moveType=CASTLE;
  // Make black and white castle rights look the same.
  ctl=(b.toMove==WHITE)?b.castle:b.castle>>2;
  if (ctl&whitekingsidecastle)
  {
    if ((b._board[sq+1]==EMPTY) && 
        (b._board[sq+2]==EMPTY))
    {
      testMove.toSquare=sq+2;
      ml.push_back(testMove);
    }
  }
  if (ctl&whitequeensidecastle)
  {
    if ((b._board[sq-1]==EMPTY) && 
        (b._board[sq-2]==EMPTY) && 
        (b._board[sq-3]==EMPTY))
    {
      testMove.toSquare=sq-2;
      ml.push_back(testMove);
    }
  }
}

void MoveGenerator::addKnightMoves(BasicBoard& b, MoveList& ml, typeSquare sq)
{
  int i=0;
  testMove.clear();
  testMove.fromSquare=sq;
  while (i<8)
  {
    if (knightMoves[sq][i]==UNDEF)
      break;
    testMove.toSquare=knightMoves[sq][i++];
    if (b._board[testMove.toSquare]==EMPTY)
    {
      ml.push_back(testMove);
    }else if (PIECECOLOR(b._board[testMove.toSquare])!=b.toMove)
    {
      testMove.moveType=CAPTURE;
      testMove.capturedpiece=b._board[testMove.toSquare];
      ml.push_back(testMove);
      testMove.moveType=0;
      testMove.capturedpiece=0;
    }
  }
}

void MoveGenerator::addKnightCaptureMoves(BasicBoard& b, MoveList& ml, typeSquare sq)
{
  int i=0;
  testMove.clear();
  testMove.fromSquare=sq;
  testMove.moveType=CAPTURE;
  while (i<8)
  {
    if (knightMoves[sq][i]==UNDEF)
      break;
    testMove.toSquare=knightMoves[sq][i++];
    if ((b._board[testMove.toSquare]!=EMPTY) && 
        (PIECECOLOR(b._board[testMove.toSquare])!=b.toMove))
    {
      testMove.capturedpiece=b._board[testMove.toSquare];
      ml.push_back(testMove);
    }
  }
}

void MoveGenerator::addPawnCaptureMoves(BasicBoard& b, MoveList& ml, typeSquare sq)
{
  int pawnRow,i,capfile;
  pawnRow=(b.toMove)?-16:16;
  testMove.clear();
  testMove.fromSquare=sq;

  // Promote is also change on material
	testMove.toSquare=testMove.fromSquare+pawnRow;
  if (b._board[testMove.toSquare]==EMPTY)
  {
    if ((testMove.toSquare<a2)||(testMove.toSquare>h7))
    {
      testMove.moveType=PAWNMOVE|PROMOTE;
      addPawnPromote(b,ml,testMove);
      testMove.clear();
      testMove.fromSquare=sq;
    }
  }

  capfile=-1; // Look first to left
  for (i=0;i<2;i++)
  {
    testMove.toSquare=sq+capfile+pawnRow;
    if (LEGALSQUARE(testMove.toSquare))
    {
      if ((b._board[testMove.toSquare]!=EMPTY) && 
          (PIECECOLOR(b._board[testMove.toSquare])!=b.toMove))
			{
        testMove.capturedpiece=b._board[testMove.toSquare];
        if ((testMove.toSquare<a2)||(testMove.toSquare>h7))
        {
          testMove.moveType=PAWNMOVE|CAPTURE|PROMOTE;
          addPawnPromote(b,ml,testMove);
        }else
        {
          testMove.moveType=PAWNMOVE|CAPTURE;
          ml.push_back(testMove);
        }
      }else if (testMove.toSquare==b.enPassant)
      {
        testMove.moveType=PAWNMOVE|CAPTURE|ENPASSANT;
        if (b.toMove==WHITE)
          testMove.capturedpiece=blackpawn;
        else
          testMove.capturedpiece=whitepawn;
        ml.push_back(testMove);
      }
    }
	  capfile=1;  // Look to the right
  }
}

// Knight and king
void MoveGenerator::addNoSlideCaptureMoves(BasicBoard& b, MoveList& ml, typeSquare sq, const int* sqadd)
{
  int i;
  testMove.clear();
  testMove.fromSquare=sq;
  testMove.moveType=CAPTURE;
  for (i=0;i<8;i++)
  {
    testMove.toSquare=sq+sqadd[i];
    if (LEGALSQUARE(testMove.toSquare))
    {
      if ((b._board[testMove.toSquare]!=EMPTY) && 
          (PIECECOLOR(b._board[testMove.toSquare])!=b.toMove))
      {
        testMove.capturedpiece=b._board[testMove.toSquare];
        ml.push_back(testMove);
      }
    }
  }
}

// Queen, bishop and rook capture
void MoveGenerator::addSlideCaptureMoves(BasicBoard& b, MoveList& ml, typeSquare sq, const int* sqadd)
{
  int i;
  testMove.clear();
  testMove.fromSquare=sq;
  testMove.moveType=CAPTURE;
  for (i=0;i<4;i++)
  {
    testMove.toSquare=sq+sqadd[i];
    while (LEGALSQUARE(testMove.toSquare))
    {
      if (b._board[testMove.toSquare]!=EMPTY)
      {
        if  (PIECECOLOR(b._board[testMove.toSquare])!=b.toMove)
        {
          testMove.capturedpiece=b._board[testMove.toSquare];
          ml.push_back(testMove);
        }
        break;
      }
      testMove.toSquare+=sqadd[i];
    }
  }
}

void MoveGenerator::undoMove(BasicBoard& b, ChessMove& m)
{
  // Move back the piece
  b._board[m.fromSquare]=b._board[m.toSquare];
  // If it was a capture set the capture piece back
  if (m.moveType&CAPTURE)
  {
    if (m.moveType&ENPASSANT)
    {
      if (b.toMove==BLACK)
        b._board[m.toSquare-16]=blackpawn;
      else
        b._board[m.toSquare+16]=whitepawn;
      b._board[m.toSquare]=EMPTY;
    }else
    {
      b._board[m.toSquare]=m.capturedpiece;
    }
  }else
  {
    b._board[m.toSquare]=EMPTY;
  }
  // Was it a promotion
  if (m.moveType&PROMOTE)
  {
    if (b.toMove==WHITE)
      b._board[m.fromSquare]=blackpawn;
    else
      b._board[m.fromSquare]=whitepawn;
  }
  // Set back the rook on castle
  if (m.moveType&CASTLE)
  {
    switch (m.toSquare)
    {
      case c1:
        b._board[d1]=EMPTY;
        b._board[a1]=whiterook;
        break;
      case g1:
        b._board[f1]=EMPTY;
        b._board[h1]=whiterook;
        break;
      case c8:
        b._board[d8]=EMPTY;
        b._board[a8]=blackrook;
        break;
      case g8:
        b._board[f8]=EMPTY;
        b._board[h8]=blackrook;
        break;
    }
  }
  // Restore board condition
  b.enPassant=m.oldEnPassant;
  b.castle=m.oldCastle;
  b.move50draw=m.oldMove50draw;
  // Give back the move
  b.toMove=OTHERPLAYER(b.toMove);
}

void MoveGenerator::doMove(BasicBoard& b, ChessMove& m)
{
  int pawnRow;

  // Housekeeping for undoMove
  m.oldCastle=b.castle;
  m.oldEnPassant=b.enPassant;
  m.oldMove50draw=b.move50draw;

  // Move piece
  if (m.moveType&PROMOTE)
    b._board[m.toSquare]=m.promotePiece;
  else
    b._board[m.toSquare]=b._board[m.fromSquare];
  b._board[m.fromSquare]=EMPTY;

  // Remove pawn on EnPassant move
  pawnRow=(b.toMove)?-16:16;
  if (m.moveType&ENPASSANT)
    b._board[m.toSquare-pawnRow]=EMPTY;

  // Set new ep for doble pawnmoves. (only if possible)
  if (m.moveType&DBLPAWNMOVE)
  {
    if (LEGALSQUARE(m.toSquare-1) &&
      (b._board[m.toSquare-1]==COLORPIECE(OTHERPLAYER(b.toMove),PAWN)))
      b.enPassant=m.toSquare-pawnRow;
    else if ((LEGALSQUARE(m.toSquare+1)) &&
       (b._board[m.toSquare+1]==COLORPIECE(OTHERPLAYER(b.toMove),PAWN)))
      b.enPassant=m.toSquare-pawnRow;
    else
      b.enPassant=UNDEF;
  }else
  {
    b.enPassant=UNDEF;
  }
  // Move rook on castle
  if (m.moveType&CASTLE)
  {
    switch (m.toSquare)
    {
      case c1:  // White queenside castle
        b._board[a1]=EMPTY;
        b._board[d1]=whiterook;
        break;
      case g1:  // White kingside castle
        b._board[h1]=EMPTY;
        b._board[f1]=whiterook;
        break;
      case c8:  // Black queenside castle
        b._board[a8]=EMPTY;
        b._board[d8]=blackrook;
        break;
      case g8:  // Black kingside castle
        b._board[h8]=EMPTY;
        b._board[f8]=blackrook;
        break;
    }
  }

  // Fifty moves rule
  if ((m.moveType&(PAWNMOVE|CAPTURE))==0)
    b.move50draw++;
  else
    b.move50draw=0;

  // Set new castle rights
  if (b._board[m.toSquare]==whiteking) // White king has moved
    b.castle&= ~(whitekingsidecastle|whitequeensidecastle);
  if (b._board[m.toSquare]==blackking) // Black king has moved
    b.castle&= ~(blackkingsidecastle|blackqueensidecastle);

  if (b.castle&whitequeensidecastle)
    if ((m.fromSquare==a1) || // a1 rook has moved or captured
        (m.toSquare==a1))
      b.castle&= ~whitequeensidecastle;

  if (b.castle&whitekingsidecastle) 
    if ((m.fromSquare==h1) || // h1 rook has moved or captured
        (m.toSquare==h1))
      b.castle&= ~whitekingsidecastle;

  if (b.castle&blackqueensidecastle)
    if ((m.fromSquare==a8) || // a8 rook has moved or captured
        (m.toSquare==a8)) 
      b.castle&= ~blackqueensidecastle;

  if (b.castle&blackkingsidecastle)
    if ((m.fromSquare==h8) || // h8 rook has moved or captured
        (m.toSquare==h8))
      b.castle&= ~blackkingsidecastle;

  // Hands over the move
  b.toMove=OTHERPLAYER(b.toMove);
}

void MoveGenerator::undoNullMove(BasicBoard& b, ChessMove& m)
{
  // Restore board condition
  b.enPassant=m.oldEnPassant;
  b.castle=m.oldCastle;
  b.move50draw=m.oldMove50draw;
  // Give back the move
  b.toMove=OTHERPLAYER(b.toMove);
}

void MoveGenerator::doNullMove(BasicBoard& b, ChessMove& m)
{
  // Housekeeping for undoNullMove
  m.oldCastle=b.castle;
  m.oldEnPassant=b.enPassant;
  m.oldMove50draw=b.move50draw;

  m.moveType=NULL_MOVE;

  b.move50draw++;
  b.enPassant=UNDEF;

  // Hands over the move
  b.toMove=OTHERPLAYER(b.toMove);
}



