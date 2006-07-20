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

#include <monapi/syscall.h>
#include <stdio.h>

extern int main(int, char*[]);

extern "C" void monapi_initialize_memory(int);
extern "C" int user_start_c_impl(FuncMain*);

FILE __sF[3];
extern "C" FILE *_open_stdin(void);
extern "C" FILE *_open_stdout(void);
extern "C" FILE *_open_stderr(void);

extern "C" int user_start()
{
	monapi_initialize_memory(64*1024*1024);
	setConstructorList(0);

	/* Initialize stdin, stdout, stderr. */
	__sF[0] = *(_open_stdin());
	__sF[1] = *(_open_stdout());
	__sF[2] = *(_open_stderr());

	/* Call _main */
	int result = user_start_c_impl(main);

	/* Close stdin, stdout, stderr. */
	fclose(&__sF[0]);
	fclose(&__sF[1]);
	fclose(&__sF[2]);

	return result;
}

extern "C" void __main()
{
}
