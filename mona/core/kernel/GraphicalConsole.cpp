/*!
    \file  GraphicalConsole.cpp
    \brief class GraphicalConsole

    class GraphicalConsole

    Copyright (c) 2003 Higepon
    All rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision$
    \date   create:2003/02/22 update:$Date$
*/

#include "GraphicalConsole.h"
#include "global.h"

extern "C" void write_font(char ch, char fc, char bc);
extern "C" void scroll_up();
extern "C" char pos_x;
extern "C" char pos_y;

/*!
    \brief initilize

    \author HigePon
    \date   create:2003/02/22 update:
*/
GraphicalConsole::GraphicalConsole() {

    pos_x    = 0;
    pos_y    = 0;
    bgcolor_ = GP_BLACK;
    chcolor_ = GP_WHITE;
    clearScreen();
}

/*!
    \brief printf
]
    \param  color background color
    \author HigePon
    \date   create:2003/03/01 update:
*/
void GraphicalConsole::setBGColor(const char color) {

    bgcolor_ = color;
    return;
}

/*!
    \brief printf

    \param  color character color
    \author HigePon
    \date   create:2003/03/01 update:
*/
void GraphicalConsole::setCHColor(const char color) {

    chcolor_ = color;
    return;
}

/*!
    \brief printf

    \param  format
    \author HigePon
    \date   create:2003/02/03 update: 2007/12/07
*/
void GraphicalConsole::printf(const char *format, ...) {
#define BUFFER_SIZE 512
    char buf[BUFFER_SIZE];
    va_list ap;
    va_start(ap, format);
    int ret = vsprintf(buf, format, ap);
    va_end(ap);
    if (ret >= BUFFER_SIZE) {
        this->print("bufer over at VesaConsole::printf");
    }
    print(buf);
}

/*!
    \brief clear screen

    \author  HigePon
    \date    create:2003/02/22 update:
*/
void GraphicalConsole::clearScreen() {

    int curx = pos_x;
    int cury = pos_y;

    for (int x = 0; x < GP_MAX_WIDTH; x++) {

        for (int y = 0; y < GP_MAX_HEIGHT - 1; y++) {

            pos_x = x;
            pos_y = y;
            write_font(' ', chcolor_, bgcolor_);
        }
    }
    pos_x = curx;
    pos_y = cury;
}

/*!
    \brief set cursor

    set cursor at (x, y)

    \param x position x
    \param y position y

    \author  HigePon
    \date    create:2003/02/22 update:
*/
void GraphicalConsole::setCursor(int x, int y) {

    if (x < 0 || y < 0 || x >= GP_MAX_WIDTH || y >= GP_MAX_HEIGHT) {
        return;
    }
    pos_x = x;
    pos_y = y;
    return;
}

void GraphicalConsole::getCursor(int* x, int* y) {

    *x = pos_x;
    *y = pos_y;
}

/*!
    \brief forward cursor

    forward cursor

    \author  HigePon
    \date    create:2003/02/22 update:2003/2/23
*/
void GraphicalConsole::forwardCursor() {

    /* forward cursor */
    pos_x++;

    /* to the next line */
    if (pos_x >= GP_MAX_WIDTH) {
        pos_x = 0;
        pos_y++;
    }

    /* scroll up */
    if (pos_y >= GP_MAX_HEIGHT) {
        scrollUp();
        pos_y--;
    }
    return;
}

/*!
    \brief backward cursor n times

    backward cursor n times

    \param n times backward

    \author  HigePon
    \date    create:2003/02/22 update:
*/
void GraphicalConsole::backwardCursor(int n) {

    for (int i = 0; i < n; i++) {
        backwardCursor();
    }
}

/*!
    \brief backward cursor

    backward cursor

    \author  HigePon
    \date    create:2003/02/22 update:
*/
void GraphicalConsole::backwardCursor() {

    /* backward cursor */
    pos_x--;

    /* back line */
    if (pos_x < 0) {
        pos_x = GP_MAX_WIDTH - 1;
        pos_y--;
    }

    /* scroll down? */
    if (pos_y < 0) {
        pos_y = 0;
    }
    return;
}


/*!
    \brief go to the next line

    go to the next line

    \author  HigePon
    \date    create:2003/02/22 update:
*/
void GraphicalConsole::newLine() {

    /* new line */
    pos_x = 0;
    pos_y++;

    /* scroll up */
    if (pos_y >= GP_MAX_HEIGHT) {
        scrollUp();
        pos_y--;
    }
}

/*!
    \brief scroll up

    scroll up

    \author  HigePon
    \date    create:2003/02/22 update:2003/03/01
*/
void GraphicalConsole::scrollUp() {

    scroll_up();
    return;
}

/*!
    \brief put charcter

    put charcter at the position of current cursor

    \param ch charcter to put

    \author  HigePon
    \date    create:2003/02/22 update:
*/
void GraphicalConsole::putCharacter(char ch) {

    /* character */
    switch(ch) {

      /* new line */
      case '\n':
        newLine();
        break;

      /* backspace */
      case '\b':
        // todo
        break;
      case '\t':
        // todo
        break;
      default:

        /* write charcter at (x, y) */
        asm volatile("subl   $0x20, %esp \n");
        write_font(ch, chcolor_, bgcolor_);
        asm volatile("addl   $0x20, %esp \n");
        forwardCursor();
    }
    return;
}

/*!
    \brief print string

    print string.
    string terminator is '\0'

    \param str string

    \author  HigePon
    \date    create:2002/07/22 update:
*/
void GraphicalConsole::print(const char* str) {

     for (; *str != '\0'; str++) {
         putCharacter(*str);
     }
     return;
}

