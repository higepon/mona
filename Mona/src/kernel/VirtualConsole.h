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

typedef char *va_list;
#define va_start(ap,last) (void)((ap)=(va_list)&(last)+sizeof(last))
#define va_arg(ap,type) (*((type*)(ap))++)
#define va_end(ap) (void)((ap)=NULL)

/*!
    abstract class VirtualConsole
*/
class VirtualConsole {

  public:
    virtual void printf(const char *format, ...) = 0;
    virtual void setBGColor(const char color)  = 0;
    virtual void setCHColor(const char color)  = 0;
    virtual void getCursor(int* x, int* y)     = 0;
    virtual void setCursor(int x, int y)       = 0;
    virtual void clearScreen()                 = 0;
};


#endif
