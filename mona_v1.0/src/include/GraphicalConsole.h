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
#define GP_MAX_HEIGHT 25

/*!
    Graphical console class
*/
class GraphicalConsole : public VirtualConsole {

  public:
    GraphicalConsole();
    void printf(const char *, ...);

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
    static char bgcolor_;
    static char chcolor_;
};


#endif
