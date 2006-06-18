#ifndef _TMPMONA_LIB_STDLIB_
#define _TMPMONA_LIB_STDLIB_

#include <monalibc/stdlib.h>
#include <monapi/syscall.h>

#define main _mona_main
#define exit _mona_exit

#ifdef NORMAL
#undef NORMAL
#endif

#ifdef __cplusplus
extern "C" {
#endif

int main(int argc, char* argv[]);

void exit(int code);

#ifdef __cplusplus
}
#endif

#endif
