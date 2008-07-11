/*
Copyright (c) 2008 Shotaro Tsuji

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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

