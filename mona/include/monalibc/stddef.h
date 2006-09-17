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

#ifndef _STDDEF_H_
#define _STDDEF_H_

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__)
	#if defined(__PTRDIFF_TYPE__)
		#ifndef _PTRDIFF_T
		#define _PTFDIFF_T
			typedef __PTRDIFF_TYPE__ ptrdiff_t;
		#endif
	#endif
	#if defined(__WCHAR_TYPE__) && (!__cplusplus)
		#ifndef _WCHAR_T
		#define _WCHAR_T
			typedef __WCHAR_TYPE__ wchar_t;
		#endif
	#endif
	#if defined(__WINT_TYPE__)
		#ifndef _WINT_T
		#define _WINT_T
			typedef __WINT_TYPE__ wint_t;
		#endif
	#endif
#else
	#ifndef _PTRDIFF_T
	#define _PTRDIFF_T
		typedef signed int ptrdiff_t;
	#endif
	#ifndef _WCHAR_T
	#define _WCHAR_T
		typedef unsigned short wchar_t;
	#endif
#endif

#define offsetof(s_name,m_name) (size_t)&(((s_name*)0)->m_name)

#ifdef __cplusplus
}
#endif


#endif
