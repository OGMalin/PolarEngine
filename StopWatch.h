#pragma once

#include <windows.h>

// A simple stop watch
class StopWatch
{
protected:
  // Time when last started/resumed
  DWORD starttime;
  // Time elapset from zero to last time stopped
  DWORD elapsed;
public:
  // True if the clock is running
  bool  ticking;
  StopWatch(){ reset(); };
  virtual ~StopWatch(){};
  // Start the clock from zero
  virtual void start();
  // Stop the clock from running
  virtual DWORD stop();
  // Read the clock (times in milliseconds)
  virtual DWORD read();
  // Start the clock without reset it first.
  virtual void resume();
  // Reset the clock.
  virtual void reset();
  // Set the clock in milliseconds.
  virtual void set(int ms);
};

