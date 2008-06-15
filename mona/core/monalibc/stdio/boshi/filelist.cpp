#include <sys/List.h>
#include <sys/HList.h>
#include "file.h"

extern "C" void* __mlibc_filelist_initializer(void *p)
{
	HList<FILE*>* list;

	list = (HList<FILE*>*)p;
	list = new HList<FILE*>(FOPEN_MAX);
	return (void*)list;
}

extern "C" int __mlibc_filelist_add(void *p, FILE* f)
{
	HList<FILE*>* list;

	if( p == NULL ) p = __mlibc_filelist_initializer(p);
	list = (HList<FILE*>*)p;
	if( list->size() >= FOPEN_MAX ) return 0;
	list->add(f);
	return 1;
}

extern "C" FILE* __mlibc_filelist_get(void *p, int n)
{
	HList<FILE*>* list;

	if( p == NULL ) p = __mlibc_filelist_initializer(p);
	list = (HList<FILE*>*)p;
	if( list->size() <= n ) return NULL;
	return list->get(n);
}

extern "C" int __mlibc_filelist_remove_by_index(void *p, int n)
{
	HList<FILE*>* list;

	if( p == NULL ) p = __mlibc_filelist_initializer(p);
	list = (HList<FILE*>*)p;
	if( list->size() <= n ) return 0;
	list->removeAt(n);
	return 1;
}

extern "C" int __mlibc_filelist_remove_by_element(void *p, FILE* f)
{
	HList<FILE*>* list;

	if( p == NULL ) p = __mlibc_filelist_initializer(p);
	list = (HList<FILE*>*)p;
	if( !list->hasElement(f) ) return 0;
	list->remove(f);
	return 1;
}

