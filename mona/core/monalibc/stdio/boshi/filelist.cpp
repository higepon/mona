/*
Copyright (c) 2008 Shotaro Tsuji

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <sys/List.h>
#include <sys/HList.h>
#include "file.h"

extern "C"
{

void* __mlibc_filelist_initializer(void *p)
{
	HList<FILE*>* list;

	list = (HList<FILE*>*)p;
	list = new HList<FILE*>(FOPEN_MAX);
	return (void*)list;
}

int __mlibc_filelist_add(void *p, FILE* f)
{
	HList<FILE*>* list;

	if( p == NULL ) p = __mlibc_filelist_initializer(p);
	list = (HList<FILE*>*)p;
	if( list->size() >= FOPEN_MAX ) return 0;
	list->add(f);
	return 1;
}

FILE* __mlibc_filelist_get(void *p, int n)
{
	HList<FILE*>* list;

	if( p == NULL ) p = __mlibc_filelist_initializer(p);
	list = (HList<FILE*>*)p;
	if( list->size() <= n ) return NULL;
	return list->get(n);
}

int __mlibc_filelist_remove_by_index(void *p, int n)
{
	HList<FILE*>* list;

	if( p == NULL ) p = __mlibc_filelist_initializer(p);
	list = (HList<FILE*>*)p;
	if( list->size() <= n ) return 0;
	list->removeAt(n);
	return 1;
}

int __mlibc_filelist_remove_by_element(void *p, FILE* f)
{
	HList<FILE*>* list;

	if( p == NULL ) p = __mlibc_filelist_initializer(p);
	list = (HList<FILE*>*)p;
	if( !list->hasElement(f) ) return 0;
	list->remove(f);
	return 1;
}

}

