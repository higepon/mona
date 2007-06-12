#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdlib.h>

void* gc_malloc_has_pointer(uint32_t size);
void* gc_realloc(void* p, size_t size);
void* gc_calloc(size_t n, size_t size);
void gc_dont_free(void* p);


#define HAVE_STRING_H 1
#define HAVE_STDARG_PROTOTYPES 1
#define SIZEOF_INT 4

#ifdef xmalloc
#undef xmalloc
#define xmalloc     gc_malloc_has_pointer
#endif

#ifdef xrealloc
#undef xrealloc
#define xrealloc     gc_realloc
#endif

#ifdef xcalloc
#undef xcalloc
#define xcalloc     gc_calloc
#endif

#ifdef xfree
#undef xfree
#define xfree     gc_dont_free
#endif

#ifdef xalloca
#undef xalloca
#define xalloca   gc_malloc_has_pointer
#endif

#endif
