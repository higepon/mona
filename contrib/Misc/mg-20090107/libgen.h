/* Author: Han Boetes <han@mijncomputer.nl> */
/* Released in public domain (duh) */

/* This is avoids some code repetition. */
/* Cygwin's dirname is currently broken. */
#if defined( __GLIBC__ ) || defined( __CYGWIN__ )
/* Nuts! */
extern char * basename(const char *path);
extern char *  dirname(const char *path);
#else
#include <libgen.h>
#endif
