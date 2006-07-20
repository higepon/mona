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

#include <string.h>
#include <errno.h>

#define EUNKNOWN_MSG		"Unknown error"
#define EPERM_MSG		"Operation not permitted"
#define EBADF_MSG		"Bad file descriptor"
#define ENOMEM_MSG		"Cannot allocate memory"
#define EINVAL_MSG		"Invalid argument"

char *strerror(int n)
{
	static char err_s[128];
	switch(n)
	{
		case EUNKNOWN:
			strlcpy(err_s, EUNKNOWN_MSG, 127);
			break;
		case EPERM:
			strlcpy(err_s, EPERM_MSG, 127);
			break;
		case EBADF:
			strlcpy(err_s, EBADF_MSG, 127);
			break;
		case ENOMEM:
			strlcpy(err_s, ENOMEM_MSG, 127);
			break;
		case EINVAL:
			strlcpy(err_s, EINVAL_MSG, 127);
			break;
		default:
			return NULL;
	}
	return err_s;
}
