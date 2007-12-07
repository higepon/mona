#include "gdb-adapter.h"
#include "VirtualConsole.h"

extern "C" VirtualConsole* g_console;

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
