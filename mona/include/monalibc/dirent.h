/*************************************************************
 * Copyright (c) 2006 Shotaro Tsuji
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is	 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *************************************************************/

/* Please send bug reports to
	Shotaro Tsuji
	4-1010,
	Sakasedai 1-chome,
	Takaraduka-si,
	Hyogo-ken,
	665-0024
	Japan
	negi4d41@yahoo.co.jp
*/

#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <monalibc/stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _dir;
typedef struct _dir DIR;

struct dirent
{
	uint32_t d_ino;		/* reserved */
	int32_t	d_fileno;	/* reserved */
	int32_t	d_off;		/* offset to next entry */
	int16_t	d_reclen;	/* this is always 24 */
	int16_t	d_type;		/* this is monapi_directoryinfo.attr */
	char	d_name[256];	/* Null terminated string. max is 255 */
};

DIR *opendir(const char *name);
struct dirent *readdir(DIR *);
void rewinddir(DIR *);
int closedir(DIR *);

long telldir(DIR *);
void seekdir(DIR *, long);

#ifdef __cplusplus
}
#endif

#endif
