/*!
    \file   vga.cpp
    \brief  functions for VGA(Display)

    libraly for VGA
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/20 update:$Date$
*/
#include <vga.h>

static int curX; /*! cursor position x */
static int curY; /*! cursor position y */

/*!
    \brief initialize vga

    before use vga library, you should call this function

    \author HigePon
    \date   create:2002/07/22 update:
*/
void _sysInitVga() {
    setCursor(0, 0);
    return;
}

/*!
    \brief clear screen

    clear screen

    \author HigePon
    \date   create:2002/07/22 update:
*/
void _sysClearScreen() {

    for (int x = 0; x <= MAX_WIDTH; x++) {
        for (int y = 0; y <= MAX_HEIGHT; y++) {
            _sysWriteVram(x, y, ' ');
        }
    }
    return;
}

/*!
    \brief print string like printf

    print string like printf thanks for 2ch

    \param format use %d %s like printf

    \author HigePon
    \date   create:2002/09/08 update:2002/09/08
*/
void _sys_printf (char* format, ...) {

    void** list = (void **)&format;

    ((char**)list) += 1;
    for (int i = 0; format[i] != '\0'; i++) {

        if (format[i] == '%') {
            i++;

            switch (format[i]) {
              case 's':
                  _sysPrint((char *)*list);
                  ((char**)list) += 1;
                  break;
              case 'd':
                  _sysPrintInt((int)*list);
                  ((int*)list) += 1;
                  break;
              case '%':
                  _sysPutCharacter('%');
                  break;
              case '\0':
                  i--;
                  break;
            }
        } else {
            _sysPutCharacter(format[i]);
        }
    }
}

/*!
    \brief direct write charcter on vram

    write charcter at (x, y) directry on vram

    \param x  postion x
    \param y  postion y
    \param ch charcter to write

    \author  HigePon
    \date    create:2002/07/22 update:
*/
void _sysWriteVram(int x, int y , char ch) {

    VRAM[MAX_WIDTH * y * 2 + x * 2] = ch;
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
void _sysPrint(char* str) {
     for (; *str != '\0'; str++) {
         _sysPutCharacter(*str);
     }
     return;
}

/*!
    \brief print string and \n

    print string and \n
    string terminator is '\0'

    \param str string

    \author  HigePon
    \date    create:2002/08/17 update:
*/
void _sysPrintln(char* str) {

    _sysPrint(str);
    _sysPutCharacter('\n');
}

/*!
    \brief print integer and \n

    print integer and \n

    \param num integer number

    \author  HigePon
    \date    create:2002/08/17 update:
*/
void _sysPrintlnInt(int num) {

    _sysPrintInt(num);
    _sysPutCharacter('\n');
}

/*!
    \brief print integer

    print integer

    \param num integer number

    \author  HigePon
    \date    create:2002/07/27 update:
*/
void _sysPrintInt(int num) {

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
        str[j] = revstr[i -1];
        i--;
    }

    /* print */
    _sysPrint(str);
    return;
}
/*!
    \brief put charcter

    put charcter at the position of current cursor

    \param ch charcter to put

    \author  HigePon
    \date    create:2002/07/22 update:
*/
void _sysPutCharacter(char ch) {

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
       _sysWriteVram(curX, curY, ch);
       forwardCursor();
    }
    return;
}

/*!
    \brief set cursor

    set cursor at (x, y)

    \param x position x
    \param y position y

    \author  HigePon
    \date    create:2002/07/22 update:
*/
void setCursor(int x, int y) {

    if (x < 0 || y < 0 || x >= MAX_WIDTH || y >= MAX_HEIGHT) {
        return;
    }
    curX = x;
    curY = y;
    return;
}

/*!
    \brief forward cursor

    forward cursor

    \author  HigePon
    \date    create:2002/07/22 update:
*/
void forwardCursor() {

    /* forward cursor */
    curX++;

    /* to the next line */
    if (curX >= MAX_WIDTH) {
        curX = 0;
        curY++;
    }

    /* scroll up */
    if (curY >= MAX_HEIGHT) {
      //        scrollUp();
    }
    return;
}

/*!
    \brief forward cursor n times

    forward cursor n times

    \param n times forward cursor

    \author  HigePon
    \date    create:2002/07/22 update:
*/
void forwardCursor(int n) {

  for (int i = 0; i < n; i++) {
      forwardCursor();
  }
}

/*!
    \brief backward cursor

    backward cursor

    \author  HigePon
    \date    create:2002/07/22 update:
*/
void backwardCursor() {

    /* backward cursor */
    curX--;

    /* back line */
    if (curX < 0) {
        curX = MAX_WIDTH - 1;
        curY--;
    }

    /* scroll down? */
    if (curY < 0) {
        curY = 0;
    }
    return;
}

/*!
    \brief backward cursor n times

    backward cursor n times

    \param n times backward

    \author  HigePon
    \date    create:2002/07/22 update:
*/
void backwardCursor(int n) {

    for (int i = 0; i < n; i++) {
        backwardCursor();
    }
}

/*!
    \brief go to the next line

    go to the next line

    \author  HigePon
    \date    create:2002/07/22 update:
*/
void newLine() {
    curX = 0;
    curY++;
}

/*!
    \brief scroll up

    scroll up

    \author  HigePon
    \date    create:2002/10/05 update:
*/
void scrollUp() {

    // copy current buffer
    for (int i = MAX_WIDTH * 2,j = 0; i <= VRAMSIZE; i++, j++) {

        VRAM[j] = VRAM[i];
    }
    // scrollup

}
