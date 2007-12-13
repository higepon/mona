#ifndef __MONA_GDB_ADAPTER__
#define __MONA_GDB_ADAPTER__

#include "vsprintf.h"

#ifdef __cplusplus
extern "C" {
#endif
void gdb_printf(const char* fmt, ...);
void putDebugChar(int ch);
int getDebugChar();
#ifdef __cplusplus
}
#endif

#define fprintf(stream, ...) gdb_printf(__VA_ARGS__)
#define printf(...) gdb_printf(__VA_ARGS__)

#endif // __MONA_GDB_ADAPTER__
