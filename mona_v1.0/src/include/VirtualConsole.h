/*!
    \file  VirtualConsole.h
    \brief class VirtualConsole

    class VirtualConsole abstract

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/02/03 update:$Date$
*/
#ifndef _MONA_VIRTUAL_CONSOLE_
#define _MONA_VIRTUAL_CONSOLE_

#include<types.h>

/*!
    abstract class VirtualConsole
*/
class VirtualConsole {

  public:
    virtual void printf(const char *, ...) = 0;
    virtual void setBGColor(const char color) = 0;
    virtual void setCHColor(const char color) = 0;
    virtual void putCharacter(char ch) = 0;
    virtual void print(char* str)  = 0;
    virtual void printInt(int num) = 0;
    virtual void putInt(size_t n, int base) = 0;
};


#endif
