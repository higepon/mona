#pragma once

#ifdef DEBUG
#define debug(x) { x }
#define dprintf(fmt, ...) { _printf(fmt, __VA_ARGS__); _logprintf(fmt, __VA_ARGS__); }
#define dputs(s){ _printf("%s\n", s); _logprintf("%s\n", s); }
#else
#define debug(x)
#define dprintf(fmt, ...)
#define dputs(s)
#endif

