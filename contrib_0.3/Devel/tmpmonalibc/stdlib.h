#ifndef _TMPMONA_LIB_STDLIB_
#define _TMPMONA_LIB_STDLIB_

#include <monalibc/stdlib.h>
#include <monapi/syscall.h>

#define main _mona_main
#define exit _mona_exit

#ifdef NORMAL
#undef NORMAL
#endif

#define RAND_MAX 65536

#ifdef __cplusplus
extern "C" {
#endif

int main(int argc, char* argv[]);

void exit(int code);
void abort();
char* getenv(const char* NAME);
int putenv(char *string);
double strtod(const char *STR, char **TAIL);
double atof(const char *s);

int rand();
void srand(unsigned seed);

#ifdef __cplusplus
}
#endif

#endif
