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

#ifndef __VIRTUALCONSOLE_H__
#define __VIRTUALCONSOLE_H__

#define GP_BLACK      0
#define GP_BLUE       1
#define GP_GREEN      2
#define GP_SKYBLUE    3
#define GP_RED        4
#define GP_PINK       5
#define GP_ORANGE     6
#define GP_GRAY       7
#define GP_DARKGRAY   8
#define GP_LIGHTBLUE  9
#define GP_LIGHTBUE  10
#define GP_LIGHTGREEN 11
#define GP_LIGHTRED   12
#define GP_DARKPINK   13
#define GP_YELLOW     14
#define GP_WHITE      15

/*!
    abstract class VirtualConsole
*/
class VirtualConsole {

  public:
    virtual void printf(const char *, ...)    = 0;
    virtual void setBGColor(const char color) = 0;
    virtual void setCHColor(const char color) = 0;
    virtual void getCursor(int* x, int* y)    = 0;
    virtual void setCursor(int x, int y)      = 0;
    virtual void clearScreen()                = 0;
};

#endif
