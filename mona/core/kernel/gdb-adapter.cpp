#include "gdb-adapter.h"
#include "VirtualConsole.h"
#include "Uart.h"

extern "C" VirtualConsole* g_console;
extern "C" Uart* g_com2;

void gdb_printf(const char* fmt, ...)
{
#define BUFFER_SIZE 512
    char buf[BUFFER_SIZE];
    va_list ap;
    va_start(ap, fmt);
    int ret = vsprintf(buf, fmt, ap);
    if (ret >= BUFFER_SIZE) {
        g_console->printf("%s over flow\n", __func__);
    } else {
        g_console->printf(buf);
    }
    va_end(ap);
}

/*!
    \brief put char to COM2

    \param ch 1byte char.
    \author  Higepon
    \date    create:2007/12/13 update:
*/
void putDebugChar(int ch)
{
    g_com2->writeChar((char)ch);
}

/*!
    \brief get char from COM2

    \return char
    \author  Higepon
    \date    create:2007/12/13 update:
*/
int getDebugChar()
{
    return g_com2->readChar();
}

extern InterruptHandlers* handlers;

/*!
    \brief register exception handler to kernel

    \param exception_number exception number
    \param exception_address address of handler
    \return char
    \author  Higepon
    \date    create:2007/12/19 update:
*/
void exceptionHandler(int exception_number, void* exception_address)
{
    InterruptHandlers ih;
    ih.number = exception_number;
    ih.handler = (void (*)())exception_address;
    handlers[exception_number] = ih;
}
