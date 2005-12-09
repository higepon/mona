#ifndef _TMPMONA_LIB_STDIO_
#define _TMPMONA_LIB_STDIO_

#include <monapi/cmemoryinfo.h>
#include <monapi/syscall.h>
#include <monalibc/stdio.h>
#include <monalibc/stdarg.h>

#ifndef EOF
#define EOF -1
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	const char *path;
	const char *mode;
	monapi_cmemoryinfo *data;
	int pos, error;
} FILE;

#define stdout NULL
#define stderr NULL

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);
int fgetc(FILE *fp);
char *fgets(char *buf, int n, FILE *fp);
size_t fread(void *buf, size_t size, size_t count, FILE *fp);
int fseek(FILE *fp, long offset, int whence);
int fscanf(FILE *fp, const char *format, ...);
int ferror(FILE *fp);
int feof(FILE *fp);
int fflush(FILE *fp);

int putchar(int ch);
int putc(int ch, FILE *fp);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);

#define perror(err) printf(err)
#define fprintf(n, ...) printf(__VA_ARGS__)
#define byte hoge_albyter	//for src/mesa/shader/grammar_mesa.h

#ifdef __cplusplus
}
#endif

#endif
