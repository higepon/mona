/* Define to 1 if you have the <setjmp.h> header file. */
#define HAVE_SETJMP_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if the system has the type `uint64_t'. */
#define HAVE_UINT64_T 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://bugs.freedesktop.org/enter_bug.cgi?product=cairo"

/* Define to the full name of this package. */
#define PACKAGE_NAME USE_cairo_INSTEAD

/* Define to the full name and version of this package. */
#define PACKAGE_STRING USE_cairo_version_OR_cairo_version_string_INSTEAD

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME USE_cairo_INSTEAD

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION USE_cairo_version_OR_cairo_version_string_INSTEAD

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* The size of `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG 8

/* The size of `size_t', as computed by sizeof. */
#define SIZEOF_SIZE_T 4

/* The size of `void *', as computed by sizeof. */
#define SIZEOF_VOID_P 4

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif

/* Define to the value your compiler uses to support the warn-unused-result
   attribute */
#define WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))

/* quick hack */
#define __builtin_popcount(x) popcount_mona(x)
#include <stdint.h>
static int popcount_mona (uint32_t mask);

#if defined(_CAIROINT_H_) && !defined(MONA_QUICK_HACK_ONCE)
#define MONA_QUICK_HACK_ONCE

static inline int popcount_mona (uint32_t mask)
{
    register int y;

    y = (mask >> 1) &033333333333;
    y = mask - y - ((y >>1) & 033333333333);
    return (((y + (y >> 3)) & 030707070707) % 077);
}

#endif
