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

#include <VirtualConsole.h>
#include <types.h>
#include <io.h>


/* define 8250 regs */
#define RDR 0
#define THR 0
#define IER 1
#define IIR 2
#define LCR 3
#define MCR 4
#define LSR 5
#define MSR 6
#define SCR 7

#define DLL 0
#define DLM 1

/* define 8250 LSR bits */
#define DATA5 0x00
#define DATA6 0x01
#define DATA7 0x02
#define DATA8 0x03
#define STOP1 0x00
#define STOP2 0x04
#define PNONE 0x00
#define PODD  0x08
#define PEVEN 0x18
#define DLAB  0x80

/* define 8250 I/O */
#define out_uart(reg,dat) outportb(UART_BASE+(reg),(dat))
#define in_uart(reg) inportb(UART_BASE+(reg))

/* define baudrate clock */
#define BAUD_CLK ( 1843200 / 16 )

/* trx format define */
#define BAUDRATE 9600
#define TRXFORMAT (DATA8|STOP1|PNONE)

/* UART BASE */
#define UART_BASE 0x3f8


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

  private:
    void putCharacter(char ch);
    void forwardCursor();
    void backwardCursor();
    void backwardCursor(int n);
    void newLine();
    void scrollUp();
    void print(char* str);
    void putInt(size_t n, int base);
    void wait();
};

#endif
