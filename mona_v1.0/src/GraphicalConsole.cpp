/*!
    \file  GraphicalConsole.cpp
    \brief class GraphicalConsole

    class GraphicalConsole

    Copyright (c) 2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/02/22 update:$Date$
*/

#include<GraphicalConsole.h>

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
    \date   create:2003/02/03 update:
*/
void GraphicalConsole::printf(const char *format, ...) {

    void** list = (void **)&format;

    ((char**)list) += 1;
    for (int i = 0; format[i] != '\0'; i++) {

        if (format[i] == '%') {
            i++;

            switch (format[i]) {
              case 's':
                  print((char *)*list);
                  ((char**)list) += 1;
                  break;
              case 'd':
                  printInt((int)*list);
                  ((int*)list) += 1;
                  break;
              case 'x':
                  print("0x");
                  putInt((int)*list, 16);
                  ((int*)list) += 1;
                  break;
              case 'c':
                  putCharacter((char)(int)(*list));
                  ((char*)list) += 1;
                  break;
              case '%':
                  putCharacter('%');
                  break;
              case '\0':
                  i--;
                  break;
            }
        } else {
            putCharacter(format[i]);
        }
    }
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
void GraphicalConsole::print(char* str) {

     for (; *str != '\0'; str++) {
         putCharacter(*str);
     }
     return;
}

/*!
    \brief print integer

    print integer

    \param num integer number

    \author  HigePon
    \date    create:2003/02/22 update:
*/
void GraphicalConsole::printInt(int num) {

    char revstr[20];
    char str[20];
    int  i = 0;
    int  j = 0;

    /* negative number */
    if (num < 0) {
        str[0] = '-';
        j++;
        num = num * -1;
    }

    /* number to buffer */
    do {
        revstr[i] = '0' + (int)(num % 10);
        num = num / 10;
        i++;
    } while (num != 0);
    revstr[i] = '\0';

    /* revert */
    for (; i >= 0; j++) {
        str[j] = revstr[i - 1];
        i--;
    }

    /* print */
    print(str);
    return;
}

/*!
    \brief print int

    \param n integer to print
    \param base ex)16

    \author  HigePon
    \date    create:2003/02/22 update:
*/
void GraphicalConsole::putInt(size_t n, int base) {

    int    power;
    size_t num = n;
    size_t ch;

    for (power = 0; num != 0; power++) {
        num /= base;
    }

    for (int j = 0; j < 8 - power; j++) {
        putCharacter('0');
    }

    for (int i = power -1; i >= 0; i--) {
        ch = n / _power(base, i);
        n %= _power(base, i);

        if (i == 0 && n > 9) {

            putCharacter('A' + n -10);
        } else if (i == 0) {

            putCharacter('0' + n);
        } else if (ch > 9) {

            putCharacter('A' + ch -10);
        } else {

            putCharacter('0' + ch);
        }
    }
}

/*!
    \brief x^y

    x^y

    \param x  x^y
    \param y  x^y
    \return x^y

    \author  HigePon
    \date    create:2003/02/22 update:
*/
size_t GraphicalConsole::_power(size_t x, size_t y) {

    size_t result = x;

    for (size_t i = 1; i < y; i++) {
        result *= x;
    }
    return result;
}
