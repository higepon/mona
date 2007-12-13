/*!
    \file  LogConsole.h
    \brief class LogConsole

    class LogConsole

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2004/03/06 update:$Date$
*/
#ifndef _MONA_LOG_CONSOLE_
#define _MONA_LOG_CONSOLE_

#include <sys/types.h>
#include "VirtualConsole.h"
#include "vsprintf.h"
#include "Uart.h"

/*!
    log console class
*/
class LogConsole : public VirtualConsole
{
public:
    LogConsole();
    void printf(const char *, ...);
    void setBGColor(const char color);
    void setCHColor(const char color);
    void getCursor(int* x, int* y);
    void setCursor(int x, int y);
    void clearScreen();

protected:
    void putCharacter(char ch);
    void forwardCursor();
    void backwardCursor();
    void backwardCursor(int n);
    void newLine();
    void scrollUp();
    void print(char* str);

    Uart* com1_;
};

#endif
