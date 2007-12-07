/*!
    \file  LogConsole.cpp
    \brief class LogConsole

    class LogConsole

    Thanks nanashi at wiki.osdev.info

    Copyright (c) 2004 Higepon
    All rights reserved.
    License=MIT/X License

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

        out_uart(THR, *str++);           /* send uint8_t */
        wait();
    }
     return;
}

void LogConsole::wait(void)
{
    volatile int wait;
    for(wait = 0; wait < 10; wait++);
}
