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

#include <sys/types.h>
#include "string.h"
/*!
    abstract class VirtualConsole
*/
class VirtualConsole {

  public:
    virtual ~VirtualConsole(){};
    virtual void printf(const char *, ...)    = 0;
    virtual void setBGColor(const char color) = 0;
    virtual void setCHColor(const char color) = 0;
    virtual void getCursor(int* x, int* y)    = 0;
    virtual void setCursor(int x, int y)      = 0;
    virtual void clearScreen()                = 0;
};


#endif
