/**
 *	monaSq 共通ヘッダ
 */
/*
 *	Copyright (c) 2006 okayu punch
 *
 *	Permission is hereby granted, free of charge, to any person
 *	obtaining a copy of this software and associated documentation
 *	files (the "Software"), to deal in the Software without restriction,
 *	including without limitation the rights to use, copy, modify, merge,
 *	publish, distribute, sublicense, and/or sell copies of the Software,
 *	and to permit persons to whom the Software is furnished to do so,
 *	subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be
 *	included in all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
 *	THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#if !defined(MONASQ_BASE_H)
#define MONASQ_BASE_H

// include monAPI
#include <monapi.h>
#include <monalibc.h>

// vsprintf
extern int monasq_vsprintf(char *buf, const char *fmt, va_list args);

// sprintf
extern int monasq_sprintf(char *buf, const char *fmt, ...);

// snprintf
extern int monasq_snprintf(char *buf, unsigned int size, const char* fmt, ...);

// printf
extern void monasq_printf(const char* fmt, ...);


// TRACE マクロ
#if defined(NDEBUG)
# define TRACE(...)			(void)0
#else
# define TRACE				monasq_printf
#endif


// SQVM::Execute() に追加したフック関数
bool monasq_break_hook(class SQVM* v);


#endif /* !defined(MONASQ_BASE_H) */
