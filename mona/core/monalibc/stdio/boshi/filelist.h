#ifndef _MLIBC_FILELIST_H_
#define _MLIBC_FILELIST_H_

#include "file.h"

#ifdef __cplusplus
extern "C" {
#endif

void* __mlibc_filelist_initializer(void *p);
int __mlibc_filelist_add(void *p, FILE* f);
FILE* __mlibc_filelist_get(void *p, int n);
int __mlibc_filelist_remove_by_index(void *p, int n);
int __mlibc_filelist_remove_by_element(void *p, FILE* f);

#ifdef __cplusplus
}
#endif

#endif

