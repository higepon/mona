/*!
  \file   Charing.h
  \brief  charing is like string

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/27
  \author  shadow

  $Revision$
  $Date$
*/
#ifndef _CHARING_
#define _CHARING_
#include <sys/types.h>

#define CHARSIZE 64

class Charing{
  
  private:
    char str[CHARSIZE];
    int length;
    const int maxsize;

  public:
    Charing();
    Charing(const char s);
    Charing(const char *s);
    ~Charing();
    int GetLength();
    int Set(const char *s);
    int Setf(const char *format, ...);
    void Reset();
    int Add(const char *s);
    int Insert(int pos, const Charing& s);
    int Remove(int pos, int width);

    Charing operator+(const Charing& s);
    Charing& operator=(const char s);
    Charing& operator=(const char *s);
    Charing& operator=(const Charing& s);
    Charing& operator+=(const Charing& s);
    operator char *();
};
#endif
