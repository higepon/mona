/*!
    \file  GraphicalConsole.h
    \brief class GraphicalConsole

    class GraphicalConsole

    at real mode
    you should
    mov ax, 0x0012
    int 0x10

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/02/22 update:$Date$
*/
#ifndef _MONA_GRAPHICAL_CONSOLE_
#define _MONA_GRAPHICAL_CONSOLE_

#include<VirtualConsole.h>
#include<monaTypes.h>

#define GP_MAX_WIDTH  80
#define GP_MAX_HEIGHT 28

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
    Graphical console class
*/
class GraphicalConsole : public VirtualConsole {

  public:
    GraphicalConsole();
    void printf(const char *, ...);
    void setBGColor(const char color);
    void setCHColor(const char color);

  private:
    void forwardCursor();
    void backwardCursor();
    void backwardCursor(int n);
    void newLine();
    void putCharacter(char ch);
    void print(char* str);
    void printInt(int num);
    void putInt(size_t n, int base);
    void setCursor(int x, int y);
    void scrollUp();
    size_t _power(size_t x, size_t y);
    void clearScreen();

  private:
    int vram_[GP_MAX_WIDTH][GP_MAX_HEIGHT];
    char bgcolor_;
    char chcolor_;
};


#endif
