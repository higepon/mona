#ifndef _MLIBC_FILE_CACHE_H_
#define _MLIBC_FILE_CACHE_H_

#include "file.h"

void __mlibc_init_cache(FILE *file, char *buf, size_t size, int mode);
int  __mlibc_is_out_of_cache(FILE *f);
int  __mlibc_cache_flush(FILE *f);
int  __mlibc_is_eof(FILE *f);
void __mlibc_move_cache_current_ptr(FILE* f);
void __mlibc_init_cache_fully(FILE *f, void *p, size_t size);
int  __mlibc_cache_load_fully(FILE *f);
void __mlibc_init_cache_line(FILE *f, void *p, size_t size);
int  __mlibc_get_line_iter(FILE *f, char *buf, size_t size, size_t rsize);
int  __mlibc_get_line(FILE *f);

extern struct cache_operators_ fullbuf_operators;
extern struct cache_operators_ linebuf_operators;

#endif

