#ifndef _MLIBC_FILELIST_H_
#define _MLIBC_FILELIST_H_

#include "file.h"

extern "C" void* __mlibc_filelist_initializer(void *p);
extern "C" int __mlibc_filelist_add(void *p, FILE* f);
extern "C" FILE* __mlibc_filelist_get(void *p, int n);
extern "C" int __mlibc_filelist_remove_by_index(void *p, int n);
extern "C" int __mlibc_filelist_remove_by_element(void *p, FILE* f);

#endif

