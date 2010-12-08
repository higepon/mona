/*	$OpenBSD: sysdef.h,v 1.15 2006/08/01 22:16:03 jason Exp $	*/

/* This file is in the public domain. */

/*
 *		POSIX system header file
 */

#include	"config.h"

#ifndef MONA
#include <sys/param.h>
#endif

#ifdef MONA
#include <sys/types.h>
typedef uint32_t uid_t;
typedef uint32_t gid_t;
typedef uint32_t mode_t;
typedef uint32_t sig_atomic_t;
typedef uint32_t uint;
#define MAXPATHLEN 1024
// todo pull up
#define ENAMETOOLONG -1
#define MAXNAMLEN 256
#define DT_DIR ATTRIBUTE_DIRECTORY
#define DT_UNKNOWN -1
#define DT_LNK -1
struct timespec
{

};
#endif

/* This is the queue.h from OpenBSD/NetBSD, works fine. The one provided by
   others is incompatible. */
#if !defined(__OPENBSD__) && !defined(__NETBSD__)
#  include "queue.h"
#else
#  include <sys/queue.h>
#endif

/* necesarry to get asprintf & friends with glibc XXX doesn't work for some
 * mysterious reason! */
#ifdef __GLIBC__
#  define _GNU_SOURCE
#  define __USE_GNU
#endif
#include <stdio.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>


#define	KBLOCK		8192	/* Kill grow.			 */
#define	GOOD		0	/* Good exit status.		 */
#define	SYMBLINK	1	/* Handle symbolic links.	 */

typedef int	RSIZE;		/* Type for file/region sizes	 */
typedef short	KCHAR;		/* Type for internal keystrokes	 */

#define MALLOCROUND(m)	(m+=7,m&=~7)	/* round up to 8 byte boundary	 */

struct fileinfo {
	uid_t		fi_uid;
	gid_t		fi_gid;
	mode_t		fi_mode;
	struct timespec fi_mtime;       /* Last modified time */
};

/* extra headers missing from glibc */
#ifdef HAVE_NOSTRLCPY
extern size_t strlcpy(char *, const char *, size_t);
#endif

#ifdef HAVE_NOSTRLCAT
extern size_t strlcat(char *, const char *, size_t);
#endif

/* not provided by glibc stdio.h */
#ifdef HAVE_NOFGETLN
extern char * fgetln(FILE *, size_t *);
#endif

/* strtonum missing on other BSD's */
#ifdef HAVE_NOSTRTONUM
extern long long strtonum(const char *, long long, long long, const char **);
#endif

#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX MAXLOGNAME
#endif
