/*!
    \file  LogConsole.cpp
    \brief class LogConsole

    class LogConsole

    Thanks nanashi at wiki.osdev.info

    Copyright (c) 2004 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2004/03/05 update:$Date$
*/

#include "LogConsole.h"
#include "global.h"


/*!
    \brief initialize

    \author HigePon
    \date   create:2004/03/05 update:
*/
LogConsole::LogConsole()
{
    int baud = BAUD_CLK / BAUDRATE;
    out_uart(LCR,DLAB | TRXFORMAT);
    wait();
    out_uart(DLL, (baud & 0xff));
    wait();
    out_uart(DLM, (baud & 0xff00) >> 8);
    wait();
    out_uart(LCR, TRXFORMAT);
    wait();
    out_uart(MCR, 0x03);
    wait();
}

/*!
    \brief printf

    \param  color background color
    \author HigePon
    \date   create:2004/03/05 update:
*/
void LogConsole::setBGColor(const char color)
{
}

/*!
    \brief printf

    \param  color character color
    \author HigePon
    \date   create:2004/03/05 update:
*/
void LogConsole::setCHColor(const char color)
{
}

/*!
    \brief printf

    \param  format
    \author HigePon
    \date   create:2004/03/05 update:
*/
void LogConsole::printf(const char *format, ...)
{
    // print((char*)g_currentThread->process->getName());

    void** list = (void **)&format;

    list++;
    for (int i = 0; format[i] != '\0'; i++)
    {
        if (format[i] == '%')
        {
            i++;
            switch (format[i]) {
              case 's':
                  print((char *)*list);
                  list++;
                  break;
              case 'd':
                  putInt(*(int*)list, 10);
                  list++;
                  break;
              case 'x':
                  print("0x");
                  putInt(*(int*)list, 16);
                  list++;
                  break;
              case 'c':
                  putCharacter((char)*(int*)(list));
                  list++;
                  break;
              case '%':
                  putCharacter('%');
                  break;
              case '\0':
                  i--;
                  break;
            }
        }
        else
        {
            putCharacter(format[i]);
        }
    }
}

void LogConsole::putInt(size_t n, int base)
{
    static char buf[256];

    int geta = 8;
    int num  = n;
    if (base == 10 && num != 0)
    {
        for (geta = 0; num; num /= 10, geta++);
    }
    else if (base == 10 && num == 0)
    {
        geta = 1;
    }

    char* p = ltona(n, buf, geta, base);
    print(p);
}

/*!
    \brief clear screen

    \author  HigePon
    \date    create:2004/03/05 update:
*/
void LogConsole::clearScreen()
{
}

/*!
    \brief set cursor

    set cursor at (x, y)

    \param x position x
    \param y position y

    \author  HigePon
    \date    create:2004/03/05 update:
*/
void LogConsole::setCursor(int x, int y)
{
}

void LogConsole::getCursor(int* x, int* y)
{
}

/*!
    \brief forward cursor

    forward cursor

    \author  HigePon
    \date    create:2004/03/05 update:
*/
void LogConsole::forwardCursor()
{
}

/*!
    \brief backward cursor n times

    backward cursor n times

    \param n times backward

    \author  HigePon
    \date    create:2004/03/05 update:
*/
void LogConsole::backwardCursor(int n)
{
}

/*!
    \brief backward cursor

    backward cursor

    \author  HigePon
    \date    create:2004/03/05 update:
*/
void LogConsole::backwardCursor()
{
}


/*!
    \brief go to the next line

    go to the next line

    \author  HigePon
    \date    create:2004/03/05 update:
*/
void LogConsole::newLine()
{
}

/*!
    \brief scroll up

    scroll up

    \author  HigePon
    \date    create:2004/03/05 update:
*/
void LogConsole::scrollUp()
{
}

/*!
    \brief put charcter

    put charcter at the position of current cursor

    \param ch charcter to put

    \author  HigePon
    \date    create:2004/03/05 update:
*/
void LogConsole::putCharacter(char ch)
{
    char str[] = " ";
    str[0] = ch;
    print(str);
}

/*!
    \brief print string

    print string.
    string terminator is '\0'

    \param str string

    \author  HigePon
    \date    create:2004/03/05 update:
*/
void LogConsole::print(char* str)
{
    int lsr;
    while (*str)
    {
        do
        {
            lsr = in_uart(LSR) & 0x20;  /* wait TxRDY */
            wait();
        } while (!lsr);

        out_uart(THR, *str++);           /* send byte */
        wait();
    }
     return;
}

void LogConsole::wait(void)
{
    volatile int wait;
    for(wait = 0; wait < 10; wait++);
}
