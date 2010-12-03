/* $FreeWRT$ */

/*-
 * Copyright (c) 2007
 *	Thorsten Glaser <tg@mirbsd.de>
 *
 * Provided that these terms and disclaimer and all copyright notices
 * are retained or reproduced in an accompanying document, permission
 * is granted to deal in this work without restriction, including un-
 * limited rights to use, publicly perform, distribute, sell, modify,
 * merge, give away, or sublicence.
 *
 * This work is provided "AS IS" and WITHOUT WARRANTY of any kind, to
 * the utmost extent permitted by applicable law, neither express nor
 * implied; without malicious intent or gross negligence. In no event
 * may a licensor, author or contributor be held liable for indirect,
 * direct, other damage, loss, or other issues arising in any way out
 * of dealing in the work, even if advised of the possibility of such
 * damage or existence of a defect, except proven that it results out
 * of said person's immediate fault when using the work as intended.
 *-
 * fgetln() wrapper for operating systems with getline()
 */

#define _GNU_SOURCE		/* for getline() */
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#if defined (__GLIBC__) || defined (__CYGWIN__)  /* FreeWRT: only build
						      this where needed */
char *fgetln(FILE *, size_t *);

char *
fgetln(FILE *stream, size_t *len)
{
	char *lb = NULL;
	size_t lbsz = 0;

	*len = getline(&lb, &lbsz, stream);
	return ((*len == (size_t)-1) ? NULL : lb);
}
#endif
