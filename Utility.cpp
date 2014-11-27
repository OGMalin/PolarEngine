/**
	* Utility.cpp
	* 13.09.2011  Init
**/

#include <Windows.h>
#include <string>
#include "Utility.h"

using namespace std;

char* WhiteSpace = " \t\n\r\v\f";

bool booleanString(const std::string& s)
{
  string cs;
  if (s.length()<1)
    return false;
  unsigned int i;
  cs="";
  for (i=0;i<s.length();i++)
    cs+=toupper(s.at(i));
  if (cs.at(0)=='T')
    return true;
  if (cs.at(0)=='F')
    return false;
  if (cs=="ON")
    return true;
  if (cs=="OFF")
    return false;
  if (isdigit(cs.at(0)))
  {
    if (cs.at(0)=='0')
      return false;
    return true;
  }
  return false;
}

char* ftoa(double d, char* buffer, int bufsize, int dec, int min)
{
  char sz[20];
  sprintf_s(sz,20,"%%%d.%df",min,dec);
  sprintf_s(buffer,bufsize,sz,d);
  return buffer;
}

const std::string ftoa(double d, int dec, int min)
{
	char buffer[256];
	ftoa(d,buffer,256,dec,min);
  return string(buffer);
}

const std::string getLastError()
{
/*
  char buf[256];
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,buf,256,NULL);
  return (string(buf));
*/
  return (string(""));
}

const std::string getProgramPath()
{
/*
	string s;
	char sz[MAX_PATH];
	if (!GetModuleFileName(NULL,sz,MAX_PATH))
		return string("");
	string::size_type size;
	s=sz;
	size=s.find_last_of('\\');
	if (size!=string::npos)
		return s.substr(0,size+1);
*/
	return string("");
};

const std::string getQuote(const string& input)
{
	std::string::size_type i1, i2;
  i1=input.find_first_of("\"\'",0);
  if (i1==string::npos)
    return "";
  if (input.at(i1)=='\'')
    i2=input.find_first_of("\'",i1+1);
  else
    i2=input.find_first_of("\"",i1+1);
  if (i2==string::npos)
    return "";
  return input.substr(i1+1,i2-i1-1);
};

const std::string getWord(const string& input, int n, char* separator)
{
	std::string::size_type i1, i2;
  int j=1;
  char* sep;
  if (!separator)
    sep=WhiteSpace;
  else
    sep=separator;
  i1=input.find_first_not_of(sep,0);
  while (j<n)
  {
     i1=input.find_first_of(sep,i1);
     if (i1==string::npos)
       return "";
     i1=input.find_first_not_of(sep,i1);
     if (i1==string::npos)
       return "";
     j++;
  };
  if (i1==string::npos)
    return "";
  i2=input.find_first_of(sep,i1);
  if (i2==string::npos)
    i2=input.length();
  return input.substr(i1,i2-i1);
}

const std::string getLine(const string& input, int n)
{
	return getWord(input,n,"\r\n");
}

const std::string lowercase(const std::string& s)
{
  string line;
	std::string::size_type i, len;
  i=0;
  len=s.length();
  while (i<len)
    line+=tolower(s.at(i++));
  return line;
}

const std::string trim(const string& s) 
{
  if(s.length() == 0)
    return s;
	std::string::size_type b = s.find_first_not_of(WhiteSpace);
	std::string::size_type e = s.find_last_not_of(WhiteSpace);
  if(b == string::npos) // No non-spaces
    return "";
  return string(s, b, e - b + 1);
}

const std::string uppercase(const std::string& s)
{
  string line;
	std::string::size_type i, len;
  i=0;
  len=s.length();
  while (i<len)
    line+=toupper(s.at(i++));
  return line;
}

bool isNumber(const string& s, int type)
{
  string::size_type e;
  if (!s.length())
    return false;
  switch (type)
  {
    case 8:  e=s.find_first_not_of("01234567"); break;
    case 10: e=s.find_first_not_of("0123456789"); break;
    case 16: e=s.find_first_not_of("0123456789abcdefABCDEF"); break;
    default: e=s.find_first_not_of("0123456789.+- "); break;
  }
  if (e==string::npos)
    return true;
  return false;
}

const std::string makeHexString(BYTE* data, int len)
{
	string s="";
	char buffer[16];
	for (int i=0;i<len;i++)
	{
		sprintf_s(buffer,16,"%02X ",data[i]);
		s+=buffer;
	}
	return trim(s);
}

char* iptoa(unsigned long ip, char* buffer, int bufsize)
{
	union 
	{
		unsigned long nip;
		struct {unsigned char a,b,c,d;} cip;
	};
	
	nip=(unsigned long) ip;
	sprintf_s(buffer,bufsize,"%u.%u.%u.%u",cip.d,cip.c,cip.b,cip.a);
	return buffer;
}

const std::string iptoa(unsigned long ip)
{
	char buffer[256];
	iptoa(ip,buffer,256);
	return string(buffer);
}

DWORD atoip(std::string s)
{
	union 
	{
		unsigned long nip;
		struct {char a,b,c,d;} cip;
	};
	std::string w;
	cip.d=atoi(getWord(s,1,".").c_str());
	cip.c=atoi(getWord(s,2,".").c_str());
	cip.b=atoi(getWord(s,3,".").c_str());
	cip.a=atoi(getWord(s,4,".").c_str());
	return nip;
}

DWORD atoip(char* sz)
{
	return atoip(string(sz));
}

bool threadRunning(HANDLE hThread)
{
	if (!hThread)
		return false;
	DWORD res=0;
	if (GetExitCodeThread(hThread, &res))
	{
		if (res==STILL_ACTIVE)
			return true;
	}
	return false;
}

std::string timeToString(const SYSTEMTIME& t)
{
	char sz[256];
	sprintf_s(sz, 256, "%02d.%02d.%d %02d:%02d:%02d", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond);
	return sz;
}

std::string timeToString(const FILETIME& t)
{
	SYSTEMTIME st;
	if (!FileTimeToSystemTime(&t, &st))
		return "";
	return timeToString(st);
}

/*
// Random generator built on MSVC++ runtime function
unsigned int MSrand32()
{
return (rand()<<17)+(rand()<<2)+(rand()&0x03);
}
*/

// Random number generator
// It is building upon Donald Knuth's description in 
// "The Art Of Computer Programming" Vol.2 third edition page 185-186
unsigned int DK1rand32()
{
	//  const unsigned int prime=2147483647;
	const unsigned int a = 48271;
	const unsigned int q = 44488;  // prime/a
	const unsigned int r = 3399;   // prime%a

	//  static unsigned int seed=time(NULL);
	static unsigned int seed = 1;
	unsigned int x;
	x = a*(seed%q) - r*(seed / q);
	seed = x;
	return x;
}

unsigned int DK2rand32()
{
	const unsigned int prime = 2147483399;
	const unsigned int a = 40692;
	const unsigned int q = 52774;
	const unsigned int r = 3791;

	//  static unsigned int seed=time(NULL);
	static unsigned int seed = 1;
	unsigned int y, x;
	y = a*(seed%q) - r*(seed / q);
	seed = y;
	x = DK1rand32();
	if (y >= x)
		return (x + prime - y - 1);
	return (x - y);
}

/*
// After idea from Agner Fog
unsigned int Mersenne32()
{
static const unsigned long N = 351, M = 175, R = 19, TEMU = 11, TEMS = 7,
TEMT = 15, TEML = 17,MATRIX_A = 0xE4BD75F5,
TEMB =0x655E5280, TEMC = 0xFFD58000;
static bool init=false;
static unsigned int mt[N];
static int mti;

if (!init)
{
//  static unsigned int seed=time(NULL);
static unsigned int seed=1;
for (mti=0; mti<N; mti++)
{
seed = seed * 29943829 - 1;
mt[mti] = seed;
}
init=true;
}

unsigned long y;

if (mti >= N)
{
// generate N words at one time
const unsigned int LOWER_MASK = (1 << R) - 1; // lower R bits
const unsigned int UPPER_MASK = -1 << R;      // upper 32-R bits
int kk, km;
for (kk=0, km=M; kk < N-1; kk++)
{
y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
mt[kk] = mt[km] ^ (y >> 1) ^ (-(signed int)(y & 1) & MATRIX_A);
if (++km >= N) km = 0;
}

y = (mt[N-1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
mt[N-1] = mt[M-1] ^ (y >> 1) ^ (-(signed int)(y & 1) & MATRIX_A);
mti = 0;
}

y = mt[mti++];

// Tempering (May be omitted):
y ^=  y >> TEMU;
y ^= (y << TEMS) & TEMB;
y ^= (y << TEMT) & TEMC;
y ^=  y >> TEML;

return y;
};
*/

/*
From Robert Hyatt's Crafty (used for test)
A 32 bit random number generator. An implementation in C of the algorithm given by
Knuth, the art of computer programming, vol. 2, pp. 26-27. We use e=32, so
we have to evaluate y(n) = y(n - 24) + y(n - 55) mod 2^32, which is implicitly
done by unsigned arithmetic.
*/
/*
unsigned int CRAFTYrand32() {
//  random numbers from Mathematica 2.0.
//  SeedRandom = 1;
//  Table[Random[Integer, {0, 2^32 - 1}]
static const unsigned long x[55] = {
1410651636UL, 3012776752UL, 3497475623UL, 2892145026UL, 1571949714UL,
3253082284UL, 3489895018UL,  387949491UL, 2597396737UL, 1981903553UL,
3160251843UL,  129444464UL, 1851443344UL, 4156445905UL,  224604922UL,
1455067070UL, 3953493484UL, 1460937157UL, 2528362617UL,  317430674UL,
3229354360UL,  117491133UL,  832845075UL, 1961600170UL, 1321557429UL,
747750121UL,  545747446UL,  810476036UL,  503334515UL, 4088144633UL,
2824216555UL, 3738252341UL, 3493754131UL, 3672533954UL,   29494241UL,
1180928407UL, 4213624418UL,   33062851UL, 3221315737UL, 1145213552UL,
2957984897UL, 4078668503UL, 2262661702UL,   65478801UL, 2527208841UL,
1960622036UL,  315685891UL, 1196037864UL,  804614524UL, 1421733266UL,
2017105031UL, 3882325900UL,  810735053UL,  384606609UL, 2393861397UL };
static int init = 1;
static unsigned long y[55];
static int j, k;
unsigned long ul;
if (init)
{
int i;
init = 0;
for (i = 0; i < 55; i++) y[i] = x[i];
j = 24 - 1;
k = 55 - 1;
}

ul = (y[k] += y[j]);
if (--j < 0) j = 55 - 1;
if (--k < 0) k = 55 - 1;
return((unsigned int)ul);
}
*/

/*
// A simple rand generator posted by Gerd Isenberg on CCC.
unsigned int GIrand32()
{
static unsigned int r = 0;
return (r = 1664525L*r + 1013904223L);
}
*/

unsigned __int64 rand64()
{
	unsigned __int64 i64, low, high;
	low = rand32();
	high = rand32();
	i64 = (high << 32) | low;
	return i64;
}

