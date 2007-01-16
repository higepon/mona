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

#ifndef __STDIO_P_H__
#define __STDIO_P_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct __sFILEX
{
	unsigned char	stds;
	fpos_t		offset;
};

enum
{
	__STDIN = 1,
	__STDOUT = 2,
	__STDERR = 3,
};

int _read(int, void*, size_t);
int _write(int,void*, size_t);
int _seek(int, fpos_t, int);

void _open_stdin(FILE*);
void _open_stdout(FILE*);
void _open_stderr(FILE*);

uint32_t writeStream(void* s, const void* buf, uint32_t size);
uint32_t readStream(void* s, void* buf, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
