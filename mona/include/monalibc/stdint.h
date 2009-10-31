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

#ifndef _STDINT_H_
#define _STDINT_H_

#include <sys/types.h>

/* from ISO/IEC 988:1999 spec */

/* 7.18.1.1 Exact-width integer types */
typedef signed char		int8_t;
typedef signed short		int16_t;
typedef signed int		int32_t;
typedef signed long long	int64_t;
#ifndef __uint8_t_defined
typedef unsigned char       uint8_t;
# define __uint8_t_defined
#endif

#ifndef __uint16_t_defined
typedef unsigned short      uint16_t;
# define __uint16_t_defined
#endif
#ifndef __uint32_t_defined
typedef unsigned int            uint32_t;
# define __uint32_t_defined
#endif
typedef unsigned long long	uint64_t;

/* 7.18.1.2 Minumum-width integer types */
typedef int8_t		int_least8_t;
typedef int16_t		int_least16_t;
typedef int32_t		int_least32_t;
typedef int64_t		int_least64_t;
typedef uint8_t		uint_least8_t;
typedef uint16_t	uint_least16_t;
typedef uint32_t	uint_least32_t;
typedef uint64_t	uint_least64_t;

/* 7.18.1.3 Fastest-width integer types */
typedef int8_t		int_fast8_t;
typedef int16_t		int_fast16_t;
typedef int32_t		int_fast32_t;
typedef int64_t		int_fast64_t;
typedef uint8_t		uint_fast8_t;
typedef uint16_t	uint_fast16_t;
typedef uint32_t	uint_fast32_t;
typedef uint64_t	uint_fast64_t;

/* 7.18.1.4 Integer types capable of hgolding object pointers */
#ifndef __uintptr_t_defined
typedef unsigned long int	uintptr_t;
typedef long int		intptr_t;
# define __uintptr_t_defined
#endif



/* 7.18.1.5 Greatest-width integer types */
typedef long long		intmax_t;
typedef unsigned long long	uintmax_t;


#if (! defined(__cplusplus)) || defined(__STDC_LIMIT_MACROS)

/* 7.18.2.1 Limits of exact-width integer types */
#define INT8_MAX	127
#define INT16_MAX	32767
#define INT32_MAX	2147483647
#define INT64_MAX	9223372036854775807LL

#define INT8_MIN	-128
#define INT16_MIN	-32768
#define INT32_MIN	(-INT32_MAX-1)
#define INT64_MIN	(-INT64_MAX-1)

#define UINT8_MAX	255
#define UINT16_MAX	65535
#define UINT32_MAX	4294967295U
#define UINT64_MAX	18446744073709551615ULL

/* 7.18.2.2 Limits of minimum-width integer types */
#define INT_LEAST8_MIN		INT8_MIN
#define INT_LEAST16_MIN		INT16_MIN
#define INT_LEAST32_MIN		INT32_MIN
#define INT_LEAST64_MIN		INT64_MIN

#define INT_LEAST8_MAX		INT8_MAX
#define INT_LEAST16_MAX		INT16_MAX
#define INT_LEAST32_MAX		INT32_MAX
#define INT_LEAST64_MAX		INT64_MAX

#define UINT_LEAST8_MAX		UINT8_MAX
#define UINT_LEAST16_MAX	UINT16_MAX
#define UINT_LEAST32_MAX	UINT32_MAX
#define UINT_LEAST64_MAX	UINT64_MAX

/* 7.18.2.3 Limits of fastest minimum-width integer types */
#define INT_FAST8_MIN		INT8_MIN
#define INT_FAST16_MIN		INT16_MIN
#define INT_FAST32_MIN		INT32_MIN
#define INT_FAST64_MIN		INT64_MIN

#define INT_FAST8_MAX		INT8_MAX
#define INT_FAST16_MAX		INT16_MAX
#define INT_FAST32_MAX		INT32_MAX
#define INT_FAST64_MAX		INT64_MAX

#define UINT_FAST8_MAX		UINT8_MAX
#define UINT_FAST16_MAX		UINT16_MAX
#define UINT_FAST32_MAX		UINT32_MAX
#define UINT_FAST64_MAX		UINT64_MAX

/* 7.18.2.4 Limits of integer types capable of holding object pointers */

#define INTPTR_MIN	INT32_MIN
#define INTPTR_MAX	INT32_MAX

#define UINTPTR_MAX	UINT32_MAX

/* 7.18.2.5 Limits of greatest-width integer types */
#define INTMAX_MIN	INT64_MIN
#define INTMAX_MAX	INT64_MAX

#define UINTMAX_MAX	UINT64_MAX

/* 7.18.3 "Other" */
#define PTRDIFF_MIN	INT32_MIN
#define PTRDIFF_MAX	INT32_MAX

#define SIZE_MAX	UINT32_MAX

#ifndef WCHAR_MIN
#define WCHAR_MIN 0
#endif

#ifndef WCHAR_MAX
#ifdef __WCHAR_MAX__
#define WCHAR_MAX __WCHAR_MAX__
#else
#define WCHAR_MAX 0x7fffffff
#endif

#endif

#if (! defined(__cplusplus)) || defined(__STDC_CONSTANT_MACROS)

/* 7.18.4 Macros for integer constants */
#define INT8_C(v)	((int8_t)v)
#define INT16_C(v)	((int16_t)v)
#define INT32_C(v)	(v ## L)
#define INT64_C(v)	(v ## LL)

#define UINT8_C(v)	((uint8_t)v)
#define UINT16_C(v)	((uint16_t)v)
#define UINT32_C(v)	(v ## UL)
#define UINT64_C(v)	(v ## ULL)

#define INTMAX_C(v)	(v ## LL)
#define UINTMAX_C(v)	(v ## ULL)

#endif /* if C++, then __STDC_CONSTANT_MACROS enables the above macros */

#endif

#endif
