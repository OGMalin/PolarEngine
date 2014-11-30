#pragma once

#include <cstdlib>

#include "ChessMove.h"

#include <assert.h>
// Max size of a movelist. If you need more. Eg. for saving games You should
// use class ChessGame instead.
const int MOVELISTSIZE=MAX_MOVE;

class MoveList
{
public:
  ChessMove nomove;
  static int compareMove( const void* m1, const void* m2 )
  {
    if (((ChessMove*)(m1))->score<((ChessMove*)(m2))->score)
      return 1;
    if (((ChessMove*)(m1))->score>((ChessMove*)(m2))->score)
      return -1;
    return 0;
  };

  int compareMove( int m1, int m2 )
  {
    if (list[m1].score<list[m2].score)
      return 1;
    if (list[m1].score>list[m2].score)
      return -1;
    return 0;
  };

  ChessMove list[MOVELISTSIZE];
  int size; // pointer to next avaiable element
  MoveList(){ clear(); };
	MoveList(const MoveList& ml){ copy(ml); };
  virtual ~MoveList(){};
  inline void clear(){ size=0;};
  int begin(){ return 0; };
  int end(){ return size; };
  bool empty(){ return (size==0); };
  void copy(const MoveList& ml)
  {
    for (int i=0;i<ml.size;i++)
      list[i]=ml.list[i];
    size=ml.size;
  }
  ChessMove& at(int n)
  {
    if (n<size)
      return list[n];
    return nomove;
  }

  inline void push_back(const ChessMove& m)
  {
    if (size<MOVELISTSIZE)
    {
      list[size]=m;
      size++;
    }
//    assert(size>=MOVELISTSIZE);
  };

  void push_front(const ChessMove& m)
  {
    int i=size;
    if (i>=MOVELISTSIZE)
      i=MOVELISTSIZE-1;
    while (i>0)
    {
      list[i]=list[i-1];
      --i;
    }
    list[0]=m;
    size++;
    if (size>MOVELISTSIZE)
      size=MOVELISTSIZE;
  };

  int find(const ChessMove& m)
  {
    int i;
    for (i=0;i<size;i++)
      if (list[i]==m)
        return i;
    return size;
  };

  // Erase n'th element (n=0 is first element)
  // Count is the number of entries to remove, if this is -1
  // The rest of the moves is deleted
  void erase(int n, int count=1)
  {
    if (n<0)
      n=0;
    if (n>=size)
      return;
    if (count<0)
    {
      size=n;
      return;
    }
    int i=n;
    while ((i+count)<size)
    {
      list[i]=list[i+count];
      ++i;
    }
    size-=count;
    if (size<0)
      size=0;
  };

  // Assignment
  MoveList& operator=(const MoveList& ml)
  {
    int i;
    for (i=0;i<ml.size;i++)
      list[i]=ml.list[i];
    size=ml.size;
    return *this;
  };

  //
  void swap(int m1, int m2)
  {
    ChessMove m;
    if (m1==m2)
      return;
    m=list[m1];
    list[m1]=list[m2];
    list[m2]=m;
  };

  //
  void sort(int start=0,int end=-1)
  {
    if (end<0)
      end=size-1;
    //qsort(&list[start],end-start+1,sizeof(ChessMove),compareMove);
    bubblesort(start,end);
    //shellsort();
    //shellsort2();
    //quicksort(start,end);
  };

  void quicksort(int start, int end)
  {
    // Run Bubblesearch for short movelists
    if ((end-start)<6)
    {
      bubblesort(start,end);
      return;
    }

    int first=start;
    int last=end;

    // Take a sample move from the midle
    ChessMove m=list[(first+last)/2];

    // 5 1 6 6 3 5 7
    //       m
    swap((first+last)/2,last);
    // 5 1 6 7 3 5 6

    // Loop through the list
    while (first<last)
    {
      // Find the first move that should be sorted after the sample move
      while ((compareMove(&list[first],&m)<1) && (first<last))
        first++;
      // Find the last move that should be sorted before the sample move
      while ((compareMove(&list[last],&m)>0) && (first<last))
        last--;
      if (first<last)
        swap(first,last);
    }
    list[end]=list[last];
    list[last]=m;
    quicksort(start,first-1);
    quicksort(last+1,end);
  }

  void bubblesort(int start,int end)
  {
    if ((end-start)<2)
    {
      if (start >= end)
        return;
      if (list[end].score>list[start].score)
        swap(start,end);
      return;
    }
    bool unsorted=true;
    int i,last=end;
    while (unsorted)
    {
      unsorted=false;
      for (i=start;i<last;i++)
      {
       if (list[i+1].score>list[i].score)
        {
          swap(i,i+1);
          unsorted=true;
        }
      }
      last--;
    }
  }

  void shellsort()
  {
    int div=size/3;
    int i;
    while(1)
    {
      i=0;
      while (1)
      {
        if ((i+div)>=size)
          break;
        if (compareMove(i,i+div)>0)
          swap(i,i+div);
        i++;
      }
      if (div==1)
        break;
      div=div/3; // div/2
      if (!div)
        div=1;
    }
  }

  void shellsort2()
  {
    int h=1,i,j;
    ChessMove m;
    while ((h*3+1)<size)
      h=h*3+1;
    while (h>0)
    {
      for (i=h-1;i<size;i++)
      {
        m=list[i];
        j=i;
        for (j=i;(j>=h)&&(compareMove(&list[j-h],&m)>0);j-=h)
          list[j]=list[j-h];
        list[j]=m;
      }
      h=h/3;
    }
  }

  //
  ChessMove& front()
  {
    return list[0];
  };

  // Removes empty moves from the list
  void trunc()
  {
    int i=0;
    while (i<size)
    {
      if (list[i].empty())
      {
        size--;
        list[i]=list[size];
      }else
      {
        i++;
      }
    }
  }

  bool exist(const ChessMove& m)
  {
    if (find(m)==size)
      return false;
    return true;
  }

  ChessMove& next(int movenr)
  {
    int i=movenr+1;
    while (i<size)
    {
      if (list[i].score>list[movenr].score)
        swap(movenr,i);
      ++i;
    }
    return list[movenr];
  }
};

