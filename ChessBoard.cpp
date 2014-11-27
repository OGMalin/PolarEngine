#include "ChessBoard.h"
#include "MoveGenerator.h"
#include "Zobrist.h"
#include "Utility.h"

ChessBoard::ChessBoard() 
  : BasicBoard(){};

ChessBoard::ChessBoard(BasicBoard& bb) 
  : BasicBoard(bb){};

ChessBoard::~ChessBoard()
{
};

bool ChessBoard::move(const char* sz)
{
  ChessMove m=getMoveFromText(sz);
  if (strcmp(sz,"null")==0)
    m.moveType=NULL_MOVE;
  else
    m=getMoveFromText(sz);
  return move(m,true);
}

bool ChessBoard::move(ChessMove& m, bool legalcheck)
{
  MoveGenerator mgen;
  if (m.moveType&NULL_MOVE)
  {
    mgen.doNullMove(*this,m);
    return true;
  }
  if (legalcheck)
    if (!mgen.isLegal(*this,m))
      return false;
  mgen.doMove(*this,m);
  return true;
}

void ChessBoard::takeback(ChessMove& m)
{
  MoveGenerator mgen;
  if (m.moveType&NULL_MOVE)
    mgen.undoNullMove(*this,m);
  else
    mgen.undoMove(*this,m);
}

std::string ChessBoard::getFen()
{
  char sz[128];
  return getFen(sz);
}

char* ChessBoard::getFen(char* szFen)
{
  int file,rank,empty,cp;
  typePiece piece;
  char c;
  char sz[16];
  cp=0;
  for (rank=7;rank>=0;rank--)
  {
    empty=0;
    for (file=0;file<8;file++)
    {
      piece=_board[SQUARE(file,rank)];
      if (piece!=EMPTY)
      {
        if (empty)
        {
          _itoa_s(empty,sz,16,10);
          szFen[cp++]=sz[0];
          empty=0;
        }
        c=getCharFromPiece(piece);
        if (c==' ')
          c='P';
        if (PIECECOLOR(piece)==BLACK)
          c=tolower(c);
        szFen[cp++]=c;
      }else
      {
        empty++;
      }
    }
    if (empty)
    {
      _itoa_s(empty,sz,16,10);
      szFen[cp++]=sz[0];
      empty=0;
    }
    if (rank)
      szFen[cp++]='/';
  }
  szFen[cp++]=' ';
  if (toMove==WHITE)
    szFen[cp++]='w';
  else
    szFen[cp++]='b';
  szFen[cp++]=' ';
  if (castle)
  {
    if (castle&whitekingsidecastle)
      szFen[cp++]='K';
    if (castle&whitequeensidecastle)
      szFen[cp++]='Q';
    if (castle&blackkingsidecastle)
      szFen[cp++]='k';
    if (castle&blackqueensidecastle)
      szFen[cp++]='q';
  }else
  {
    szFen[cp++]='-';
  }
  szFen[cp++]=' ';
  if (enPassant!=UNDEF)
  {
    szFen[cp++]=(char)(FILE(enPassant)+'a');
    szFen[cp++]=(char)(RANK(enPassant)+'1');
  }else
  {
    szFen[cp++]='-';
  }
  szFen[cp++]=' ';
  szFen[cp++]='0';
  szFen[cp++]=' ';
  szFen[cp++]='1';
  szFen[cp]='\0';
  return szFen;
}

void ChessBoard::setFen(const char* szFen)
{
  char fen[256];
  strcpy_s(fen,256,szFen);
  int file=0,row=7;
  unsigned char piece;
  char* psz=fen;
  clear();
// Get pieces (first substring)
  if (!*psz) return;
  while(*psz<=' ')
  {
    ++psz;
    if (!*psz) return;
  }
  while (*psz>' ')
  {
    piece=EMPTY;
 	  switch (*psz)
    {
    	case 'P': piece=whitepawn;   break;
    	case 'N': piece=whiteknight; break;
     	case 'B': piece=whitebishop; break;
     	case 'R': piece=whiterook;   break;
     	case 'Q': piece=whitequeen;  break;
     	case 'K': piece=whiteking;   break;
     	case 'p': piece=blackpawn;   break;
     	case 'n': piece=blackknight; break;
     	case 'b': piece=blackbishop; break;
     	case 'r': piece=blackrook;   break;
     	case 'q': piece=blackqueen;  break;
     	case 'k': piece=blackking;   break;
      case '.': ++file;break;
      case '0': case '1': case '2': case '3': case '4': 
      case '5': case '6': case '7': case '8': case '9':
        if ((*(psz+1)>='0')&&(*(psz+1)<='9'))
        {
          file+=(*psz-'0')*10;
          ++psz;
          file+=(*psz-'0');
        }else
        {
         	file+=*psz-'0';
        }
        break;
      case '/':
        if (file>0)
          file=8;
        break;
    }
    if (piece!=EMPTY)
    {
      _board[SQUARE(file,row)]=piece;
      ++file;
    };
    ++psz;
    if (file>7)
    {
      row-=file/8;
      file=file%8;
    }
    if (row < 0)
    	break;
  }
// Who to move
  if (!*psz) return;
  while(*psz<=' ')
  {
    ++psz;
    if (!*psz) return;
  }
  if (tolower(*psz)=='b')
 	  toMove=BLACK;
  else
 	  toMove=WHITE;
  ++psz;
// Castle
  if (!*psz) return;
  while(*psz<=' ')
  {
    ++psz;
    if (!*psz) return;
  }
  while (*psz>' ')
  {
 	  switch (*psz)
    {
    	case 'Q':
        castle|=whitequeensidecastle;
       	break;
    	case 'K':
        castle|=whitekingsidecastle;
       	break;
    	case 'q':
        castle|=blackqueensidecastle;
       	break;
    	case 'k':
        castle|=blackkingsidecastle;
       	break;
    }
    psz++;
  }
// Enpassant
  if (!*psz) return;
  while(*psz<=' ')
  {
    ++psz;
    if (!*psz) return;
  }
  file=0,row=0;
  if ((*psz>='a') && (*psz<='h'))
    file=*psz-'a';
  ++psz;
  if ((*psz>='1') && (*psz<='8'))
    row=*psz-'1';
  if (row>0)
    enPassant=SQUARE(file,row);
  else
    enPassant=UNDEF;
};

char* ChessBoard::stripMoveText(char* mt)
{
  int i=0,j=0;
  while (!isFileChar(mt[i]) && !isPieceChar(mt[i]) && (mt[i]!='O'))
  {
    if (mt[i]=='\0')
    {
      mt[0]='\0';
      return mt;
    }
    i++;
  }
  while (mt[i])
  {
    // Xboard protocol is allowing piecechar to be in non capital letter.
    // when promoting piece is assigned. (e7e8q)
    if (isFileChar(mt[i]) || isRowChar(mt[i]) || isPieceChar(toupper(mt[i])) || mt[i]=='O')
    {
      mt[j]=mt[i];
      j++;
    }
    i++;
  }
  mt[j]='\0';
  // For a move like exd6 e.p. the last 'e' would still exist in the movetext (ed6e).
  if (j<2)
  {
    mt[0]='\0';
  }else if ((j>2)&&(mt[j-1]=='e'))
  {
    mt[j-1]='\0';
  }
  return mt;
}

void ChessBoard::fixMove(ChessMove& m)
{
  MoveList ml;
  MoveGenerator mgen;
  int moveit;

  mgen.makeMoves(*this,ml);
  moveit=0;
  while (moveit!=ml.size)
  {
    if ((ml.list[moveit].fromSquare==m.fromSquare) &&
        (ml.list[moveit].toSquare==m.toSquare) &&
        (ml.list[moveit].promotePiece==m.promotePiece))
    {
      m=ml.list[moveit];
      return;
    }
    ++moveit;
  }
  m.clear();
}

int ChessBoard::movecount()
{
  MoveList ml;
  MoveGenerator mgen;

  mgen.makeMoves(*this,ml);
  return ml.size;
}

const ChessMove ChessBoard::getMoveFromText(const std::string text)
{
  int moveit;
  MoveList ml;
  MoveList allmoves;
  MoveGenerator mgen;
  ChessMove m;
  typePiece piece,ppiece=EMPTY;
  char mt[20];
  int fRow=-1,fFile=-1,tRow=-1,tFile=-1;
	size_t len, i;
  if (text.length()>19)
    strcpy_s(mt,20,text.substr(0,18).c_str());
  else
    strcpy_s(mt,20,text.c_str());
  i=0;
  while (mt[i]) // uniform castle
  {
    if (mt[i]=='0')
      mt[i]='O';
    if (mt[i]=='o')
      mt[i]='O';
    i++;
  }
  stripMoveText(mt);
  len=strlen(mt);
  if (len<2)
  {
    m.score=1;
    return m; // Move is an empty move by default.
  }
// Castle
  if (strncmp(mt,"OO",2)==0)
  {
    fFile=4;
    if (strncmp(mt,"OOO",3)==0)
      tFile=2;
    else
      tFile=6;
    if (toMove==WHITE)
    {
      fRow=0;
      tRow=0;
    }else
    {
      fRow=7;
      tRow=7;
    }
    piece=KING;
  }else
  {
    piece=getPieceFromChar(mt[0]);
    i=len-1;
    if (isPieceChar(toupper(mt[i])))
    {
      ppiece=getPieceFromChar(toupper(mt[i]));
      i--;
    }
    while (i>=0)
    {
      if (isFileChar(mt[i]))
      {
        if (tFile==-1)
          tFile=mt[i]-'a';
        else
          fFile=mt[i]-'a';
      }
      if (isRowChar(mt[i]))
      {
        if (tRow==-1)
          tRow=mt[i]-'1';
        else
          fRow=mt[i]-'1';
      }
      i--;
    }
    if ((fFile>7) || (fRow>7) || (tFile>7) || (tRow>7))
      return m;
    if ((piece==EMPTY) && (fFile==-1))
      fFile=tFile;
    if ((piece==PAWN) && (fFile==-1))
      fFile=tFile;
    if ((fFile>=0) && (fRow>=0))
      piece=PIECE(_board[SQUARE(fFile,fRow)]);
    if (piece==EMPTY)
      piece=PAWN;
  }
  
  // Force queen promotion if promotion piece is missing.
  if ((piece==PAWN) && ((tRow==7) || (tRow==0)) && (ppiece==EMPTY))
    ppiece=(toMove==WHITE)?whitequeen:blackqueen;

  piece=PIECE(piece);
  piece=(toMove==WHITE)?piece:piece+6;
  mgen.makeMoves(*this,allmoves);
  moveit=0;
  ml.clear();
  while (moveit!=allmoves.size)
  {
    m=(allmoves.list[moveit]);
    if (piece==_board[m.fromSquare])
    {
      if (fFile>=0)
      {
        if (fFile!=FILE(m.fromSquare))
        {
          ++moveit;
          continue;
        }
      }
      if (tFile>=0)
      {
        if (tFile!=FILE(m.toSquare))
        {
          ++moveit;
          continue;
        }
      }
      if (fRow>=0)
      {
        if (fRow!=RANK(m.fromSquare))
        {
          ++moveit;
          continue;
        }
      }
      if (tRow>=0)
      {
        if (tRow!=RANK(m.toSquare))
        {
          ++moveit;
          continue;
        }
      }
      if (ppiece!=m.promotePiece)
      {
        ++moveit;
        continue;
      }
      ml.push_back(m);
    }
    ++moveit;
  };
  if (ml.size==1)
  {
    m=(ml.list[ml.begin()]);
  }else
  {
    m.clear();
    m.score=2;
  }
  return m;
};

const ChessMove ChessBoard::findMove(const BasicBoard& bb)
{
  MoveList ml;
  MoveGenerator mgen;
  mgen.makeMoves(*this,ml);
  for (int i=0;i<ml.size;i++)
  {
    if (move(ml.list[i]))
    {
      if (compare(bb)!=1)
      {
        takeback(ml.list[i]);
        return ml.list[i];
      }
      takeback(ml.list[i]);
    }
  }
  ChessMove m;
  return m;
}

bool ChessBoard::isRowChar(char c)
{
  switch (c)
  {
    case '1' : case '2' : case '3' : case '4' :
    case '5' : case '6' : case '7' : case '8' :
      return true;
  }
  return false;
}

bool ChessBoard::isFileChar(char c)
{
  switch (c)
  {
    case 'a' : case 'b' : case 'c' : case 'd' :
    case 'e' : case 'f' : case 'g' : case 'h' :
      return true;
  }
  return false;
}

bool ChessBoard::isPieceChar(char c)
{
  switch (c)
  {
    case 'N' : case 'B' : case 'R' : case 'Q' : case 'K' :
      return true;
  }
  return false;
}

typePiece ChessBoard::getPieceFromChar(char c)
{
  int cl;
  if (toMove==BLACK)
    cl=6;
  else
    cl=0;
  switch (c)
  {
    case 'N' :
      return cl+KNIGHT;
    case 'B' :
      return cl+BISHOP;
    case 'R' :
      return cl+ROOK;
    case 'Q' :
      return cl+QUEEN;
    case 'K' :
      return cl+KING;
    default :
      return EMPTY;
  };
};

char ChessBoard::getCharFromPiece(typePiece p)
{
  switch (PIECE(p))
  {
    case KNIGHT :
      return 'N';
    case BISHOP :
      return 'B';
    case ROOK :
      return 'R';
    case QUEEN :
      return 'Q';
    case KING :
      return 'K';
    default :
      return ' ';
  };
};

char* ChessBoard::makeMoveText(const ChessMove& cm, char* buf, int bufsize, int type)
{
  int it,cnt,f,r;
  int p=0;
  typePiece piece;
  std::string mt;
  ChessMove m;
  buf[0]='\0';

  // Be sure that all field in the move is filled (capture, ep etc.)
  mt='a'+FILE(cm.fromSquare);
  mt+='1'+RANK(cm.fromSquare);
  mt+='a'+FILE(cm.toSquare);
  mt+='1'+RANK(cm.toSquare);
  if (cm.moveType&PROMOTE)
    mt+=tolower(getCharFromPiece(cm.promotePiece));
  m=getMoveFromText(mt);
  if (m.empty())
    return buf;
  m.score=cm.score;
  
  // For coordinate system there isn't needed more work
  if (type==COOR)
  {
    strcpy_s(buf,bufsize,mt.c_str());
    return buf;
  }

  MoveList ml1,ml2;
  MoveGenerator mg;
  piece=PIECE(_board[m.fromSquare]);
  switch (piece)
  {
    case PAWN   :
      if ((type==LAN)||(m.moveType&CAPTURE))
      {
        buf[p++]='a'+FILE(m.fromSquare);
        if (type==LAN)
        {
          buf[p++]='1'+RANK(m.fromSquare);
        }
      }
      break;
    case KNIGHT :
    case BISHOP :
    case ROOK   :
    case QUEEN  :
      buf[p++]=getCharFromPiece(piece);
      if (type==LAN)
      {
        buf[p++]='a'+FILE(m.fromSquare);
        buf[p++]='1'+RANK(m.fromSquare);
        break;
      }
      mg.makeMoves(*this,ml1);
      it=0;
      while (it<ml1.size)
      {
        if ((ml1.list[it].toSquare==m.toSquare) && (PIECE(_board[ml1.list[it].fromSquare])==piece))
          ml2.push_back(ml1.list[it]);
        it++;
      };
      if (ml2.size>1)
      {
        f=FILE(m.fromSquare);
        r=RANK(m.fromSquare);
        it=0;
        cnt=0;
        while (it<ml2.size)
        {
          if (FILE(ml2.list[it].fromSquare)==f)
            cnt++;
          it++;
        };
        if (cnt==1)
        {
          buf[p++]='a'+f;
          break;
        }
        buf[p++]='1'+r;
      }
      break;
    case KING   :
      if (m.moveType&CASTLE)
      {
        strcpy_s(buf,bufsize,"O-O");
        if (FILE(m.toSquare)==2)
          strcat_s(buf,bufsize,"-O");
        mg.doMove(*this,m);
        if (mg.inCheck(*this,toMove))
          strcat_s(buf,bufsize,"+");
        mg.undoMove(*this,m);
        return buf;
      }
      buf[p++]='K';
      if (type==LAN)
      {
        buf[p++]='a'+FILE(m.fromSquare);
        buf[p++]='1'+RANK(m.fromSquare);
      }
      break;
  }

  if (m.moveType&CAPTURE)
    buf[p++]='x';
  else if (type==LAN)
    buf[p++]='-';

  buf[p++]='a'+FILE(m.toSquare);
  buf[p++]='1'+RANK(m.toSquare);
  if (m.moveType&PROMOTE)
  {
    if (type==SAN)
      buf[p++]='=';
    buf[p++]=getCharFromPiece(m.promotePiece);
  }
  mg.doMove(*this,m);
  if (mg.inCheck(*this,toMove))
  {
    mg.makeMoves(*this,ml1);
    if (ml1.size==0)
      buf[p++]='#';
    else
      buf[p++]='+';
  }
  mg.undoMove(*this,m);
  if ((type==FIDE) && (m.moveType&ENPASSANT))
  {
//    buf[p++]=' '; // Could gives unwanted linebreak
    buf[p++]='e';
    buf[p++]='.';
    buf[p++]='p';
    buf[p++]='.';
  }
  buf[p++]='\0';
  return buf;
}

bool ChessBoard::isLegal()
{
  int K=0,k=0,P=0,p=0,R=0,r=0,N=0,n=0,B=0,b=0,Q=0,q=0,sqK,sqk;
  for (int i=0;i<64;i++)
  {
    if (_board[SQUARE128(i)]==whiteking)
      ++K,sqK=SQUARE128(i);
    if (_board[SQUARE128(i)]==blackking)
      ++k,sqk=SQUARE128(i);
    if (_board[SQUARE128(i)]==whitepawn)
      ++P;
    if (_board[SQUARE128(i)]==blackpawn)
      ++p;
    if (_board[SQUARE128(i)]==whiteknight)
      ++N;
    if (_board[SQUARE128(i)]==blackknight)
      ++n;
    if (_board[SQUARE128(i)]==whitebishop)
      ++B;
    if (_board[SQUARE128(i)]==blackbishop)
      ++b;
    if (_board[SQUARE128(i)]==whiterook)
      ++R;
    if (_board[SQUARE128(i)]==blackrook)
      ++r;
    if (_board[SQUARE128(i)]==whitequeen)
      ++Q;
    if (_board[SQUARE128(i)]==blackqueen)
      ++q;
  }
  if (K!=1) return false;
  if (k!=1) return false;
  if (P>8) return false;
  if (p>8) return false;
  if (Q>9) return false;
  if (q>9) return false;
  if (R>9) return false;
  if (r>9) return false;
  if (B>9) return false;
  if (b>9) return false;
  if (N>9) return false;
  if (n>9) return false;
  if (squaredistance(sqK,sqk)<2) return false;
  return true;
}

int ChessBoard::gameEnd()
{
  MoveGenerator mg;
  MoveList ml;
  mg.makeMoves(*this,ml);
  if (!ml.size)
  {
    if (mg.inCheck(*this,toMove))
      return 1;
    return 2;
  }
  return 0;
}

HASHKEY ChessBoard::getHashkey()
{
  InitZobristKeys();

  HASHKEY key=(HASHKEY)0;

  // Pieces
  typeSquare sq=0;
  while (1)
  {
    if (_board[sq]!=EMPTY)
      key^=ZobristKey[_board[sq]-1][SQUARE64(sq)];
    sq++;
    if (sq&8)
    {
      sq+=8;
      if (sq>127)
        break;
    }
  }

  // castle could be any values from 0 to 15
  key^=ZobristKey[12][castle];

  // EnPassant
  if(enPassant!=UNDEF)
    key^=ZobristKey[12][17+FILE(enPassant)];

  // Set color to move
  if (toMove==BLACK)
    key^=ZobristKey[12][16];
  
  return key;
}

bool ChessBoard::insufficientMaterial()
{
  typeSquare sq=0;
  int mp=0;

  while (1)
  {
    switch (_board[sq])
    {
      case whitepawn:
      case blackpawn:
      case whiterook:
      case blackrook:
      case whitequeen:
      case blackqueen:
        return false;
      case whiteknight:
      case blackknight:
      case whitebishop:
      case blackbishop:
        if (mp)
          return false;
        ++mp;
        break;
      default:
        break;
    }
    ++sq;
    if (sq&8)
    {
      sq+=8;
      if (sq>127)
        break;
    }
  }
  return true;
}