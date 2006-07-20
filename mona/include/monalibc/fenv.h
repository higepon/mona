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

/* まだ作ってない */

#ifndef _FENV_H_
#define _FENV_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	unsigned short int __control;
	unsigned short int __status;
} fenv_t;

typedef unsigned short fexcept_t;

#define FE_INEXACT	0x0001
#define FE_DIVBYZERO	0x0002
#define FE_UNDERFLOW	0x0004
#define FE_OVERFLOW	0x0008
#define FE_INVALID	0x0010
#define FE_ALL_EXCEPT	0x001F

#define FE_TONEAREST	0x0001
#define FE_TOWARDZERO	0x0002
#define FE_UPWARD	0x0003
#define FE_DOWNWARD	0x0004

extern const fenv_t _FE_DFL_ENV;
#define FE_DFL_ENV &_FE_DFL_ENV

extern int fesetround(int);

#ifdef __cplusplus
}
#endif

#endif
