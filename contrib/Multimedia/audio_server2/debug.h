#pragma once

void _debug_func_enter(const char *s);
void _debug_func_leave(const char *s);

#ifdef DEBUG
#define debug(x) { x }
#define dprintf(fmt, ...) { _printf(fmt, __VA_ARGS__); _logprintf(fmt, __VA_ARGS__); }
#define dputs(s){ _printf("%s\n", s); _logprintf("%s\n", s); }

//#define denter _debug_func_enter(__func__);
//#define dleave _debug_func_leave(__func__);
#define denter
#define dleave
#else
#define debug(x)
#define dprintf(fmt, ...)
#define dputs(s)
#define denter
#define dleave
#endif

