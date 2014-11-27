#pragma once

typedef int typeCastle;
typedef int typeColor;
typedef int typePiece;
typedef int typeSquare;

typedef unsigned __int64 HASHKEY;

const int MAX_MOVE=1024; // Max move in one position
const int MAX_PLY=100;   // Max plies in one searchthread
const int MAX_MATE=1000; // Max distance to mate
const int MATE=32767;    // Mate value

// Shortcuts for squares in a 88 board
enum { a1=0x00, b1, c1, d1, e1, f1, g1, h1,
       a2=0x10, b2, c2, d2, e2, f2, g2, h2,
       a3=0x20, b3, c3, d3, e3, f3, g3, h3,
       a4=0x30, b4, c4, d4, e4, f4, g4, h4,
       a5=0x40, b5, c5, d5, e5, f5, g5, h5,
       a6=0x50, b6, c6, d6, e6, f6, g6, h6,
       a7=0x60, b7, c7, d7, e7, f7, g7, h7,
       a8=0x70, b8, c8, d8, e8, f8, g8, h8,
       UNDEF=0x88 };
enum { whitekingsidecastle= 0x01,
       whitequeensidecastle=0x02,
       blackkingsidecastle= 0x04,
       blackqueensidecastle=0x08 };
enum { EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum { WHITE, BLACK };
enum { whitepawn=1, whiteknight, whitebishop, whiterook, whitequeen, whiteking,
       blackpawn,   blackknight, blackbishop, blackrook, blackqueen, blackking };
enum { CAPTURE     = 0x01,
       ENPASSANT   = 0x02,
       CASTLE      = 0x04,
       PAWNMOVE    = 0x08,
       DBLPAWNMOVE = 0x10,
       PROMOTE     = 0x20,
       NULL_MOVE   = 0x40};

// Some useful macros
#define ILLEGALSQUARE(sq)  ((sq&0x88)!=0)
#define LEGALSQUARE(sq)    ((sq&0x88)==0)
#define PIECE(p)           (p>6?p-6:p)
#define PIECECOLOR(p)      (p>6?1:0)
#define COLORPIECE(c,p)    (c?PIECE(p)+6:PIECE(p))
#define RANK(sq)           (sq>>4)
#define FILE(sq)           (sq&0x0f)
#define SQUARE(f,r)        (f+(r<<4))
// Convert a square from a 64 square board to the 0x88 board
#define SQUARE128(sq)      ((sq&0x07)+((sq&0xf8)<<1))
// Convert a square from the 0x88 board to a 64 square board
#define SQUARE64(sq)       ((sq&0x07)+((sq&0xf8)>>1))
// Check to see a square color, Could be used in evaluation for bishops etc.
//#define WHITESQUARE(sq)    ((diagonals[sq]&0x2aaa)?1:0)
//#define BLACKSQUARE(sq)    ((diagonals[sq]&0x5555)
#define SQUARECOLOR(sq)    ((diagonals[sq]&0x5555)?BLACK:WHITE)
//#define SQUARECOLOR(sq)    (((RANK(sq)+FILE(sq))%2)?WHITE:BLACK)
// Switch player
#define OTHERPLAYER(c)     (c==WHITE?BLACK:WHITE)
// Startposition for a new game.
#define STARTPOSITION      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
// Piece
