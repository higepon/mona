#ifndef CONFIG_H
#define CONFIG_H

#define HAVE_STRING_H 1
#define HAVE_STDARG_PROTOTYPES 1
#define SIZEOF_INT 4

#define xmalloc     gc_malloc
#define xrealloc    gc_realloc
#define xcalloc     gc_calloc
#define xfree       gc_dont_free

#endif
