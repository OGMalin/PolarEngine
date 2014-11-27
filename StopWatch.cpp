// *************************************************************************
// File:        StopWatch.cpp
// Include:     StopWatch.h
// Author:      Odd Gunnar Malin
// Description: A general stopwatch
// Changes:     
// *************************************************************************

#include <windows.h>
#include "StopWatch.h"

void StopWatch::start()
{
  elapsed=0;
  ticking=true;
  starttime=GetTickCount();
}

void StopWatch::reset()
{
  starttime=0;
  elapsed=0;
  ticking=false;
}

DWORD StopWatch::stop()
{
  if (!ticking)
    return elapsed;
  ticking=false;
  DWORD r=GetTickCount();
  if (r<starttime)
    elapsed += r-(0xffffffff-starttime);
  else
    elapsed += r-starttime;
  return elapsed;
}

void StopWatch::resume()
{
  starttime=GetTickCount();
  ticking=true;
}

void StopWatch::set(int ms)
{
  if (ms<0)
    elapsed=0;
  else
    elapsed=ms;
  if (ticking)
    starttime=GetTickCount();
}

DWORD StopWatch::read()
{ 
  if (!ticking)
    return elapsed;
  DWORD r=GetTickCount();
  if (r<starttime)
    return (r-(0xffffffff-starttime)+elapsed);
  else
    return (r-starttime+elapsed);
};

