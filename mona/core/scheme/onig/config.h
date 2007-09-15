#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdlib.h>

#ifdef USE_MONA_GC
void* gc_malloc_has_pointer(uint32_t size);
void* gc_realloc(void* p, size_t size);
void* gc_calloc(size_t n, size_t size);
void gc_dont_free(void* p);
#endif

#ifdef USE_BOEHM_GC
void* GC_malloc(size_t size);
void* GC_realloc(void* ptr, size_t size);
#endif

#define HAVE_STRING_H 1
#define HAVE_STDARG_PROTOTYPES 1
#define SIZEOF_INT 4

#ifdef xmalloc
#undef xmalloc

#ifdef USE_MONA_GC
#define xmalloc     gc_malloc_has_pointer
#else
#define xmalloc     malloc
#endif

#endif


#ifdef xrealloc
#undef xrealloc

#ifdef USE_MONA_GC
#define xrealloc     gc_realloc
#elif defined(USE_BOEHM_GC)
#define xrealloc     GC_realloc
#else
#define xrealloc     realloc
#endif

#endif


#ifdef xcalloc
#undef xcalloc

#ifdef USE_MONA_GC
#define xcalloc     gc_calloc
#elif defined(USE_BOEHM_GC)
#define xcalloc(a, b)     GC_malloc(a * b)
#else
#define xcalloc     calloc
#endif

#endif


#ifdef xfree
#undef xfree

#ifdef USE_MONA_GC
#define xfree     gc_dont_free
#elif defined(USE_BOEHM_GC)
void dont_free();
#define xfree     dont_free
#else
#define xfree     free
#endif

#endif


#ifdef xalloca
#undef xalloca
#ifdef DUSE_MONA_GC
#define xalloca   gc_malloc_has_pointer
#else
#define xalloca   alloca
#endif

#endif

#endif
