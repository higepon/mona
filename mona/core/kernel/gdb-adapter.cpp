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

void putDebugChar(int ch)
{
    g_com2->writeChar((char)ch);
}

int getDebugChar()
{
    return g_com2->readChar();
}
