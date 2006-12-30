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

#include <stdlib.h>
#include <errno.h>
#include <monapi/messages.h>
#include "dirent_p.h"

struct dirent *readdir(DIR *dir)
{
	static struct dirent ent;
	struct dirent **result;
	if( readdir_r(dir, &ent, result) != 0 )
	{
		return NULL;
	}
	return &ent;
}

int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
{
	monapi_directoryinfo *di;
	if( entry == NULL || dirp == NULL )
	{
		result = NULL;
		return -1;
	}
	di = getDirInfo(dirp->cmi, dirp->index++);
	if( di == NULL )
	{
		result = NULL;
		return -1;
	}
	convertFromDirInfo(di, entry);
	result = &entry;
	return 0;
}
