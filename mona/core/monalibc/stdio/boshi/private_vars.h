#ifndef _MLIBC_PRIVATE_VARS_H_
#define _MLIBC_PRIVATE_VARS_H_

#include "file.h"

extern struct cache_operators_ fullbuf_operators;
extern struct cache_operators_ linebuf_operators;
extern struct __mlibc_file_operators_ __file_ops;
extern void* opening_files;

#endif

