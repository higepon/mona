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
  unsigned short __control_uint16_t;
  unsigned short __unused0;
  unsigned short __status_uint16_t;
  unsigned short __unused1;
  unsigned short __tag_uint16_t;
  unsigned short __unused2;  
  unsigned int	 __ip_offset;    /* instruction pointer offset */
  unsigned short __ip_selector;  
  unsigned short __opcode;
  unsigned int	 __data_offset;
  unsigned short __data_selector;  
  unsigned short __unused3;
} fenv_t;

#define FE_PC64_ENV ((const fenv_t *)-1)
#define FE_PC53_ENV ((const fenv_t *)-2)
#define FE_DFL_ENV ((const fenv_t *)0)


#define FE_INEXACT	0x0001
#define FE_DENORMAL	0x0002
#define FE_DIVBYZERO	0x0004
#define FE_UNDERFLOW	0x0008
#define FE_OVERFLOW	0x0010
#define FE_INVALID	0x0020
#define FE_ALL_EXCEPT	0x002F

#define FE_TONEAREST	0x0000
#define FE_TOWARDZERO	0x0400
#define FE_UPWARD	0x0800
#define FE_DOWNWARD	0x0c00

typedef unsigned short fexcept_t;

extern int feclearexcept(int);
extern int fegetexceptflag(fexcept_t *, int);
extern int feraiseexcept(int);
extern int fesetexceptflag(const fexcept_t *, int);
extern int fetestexcept(int);

extern int fegetround(void);
extern int fesetround(int);

extern int fegetenv(fenv_t *);
extern int fesetenv(const fenv_t *);
extern int feupdateenv(const fenv_t *);
extern int feholdexcept(fenv_t *);

#ifdef __cplusplus
}
#endif

#endif
