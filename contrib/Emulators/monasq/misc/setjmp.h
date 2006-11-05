//
// setjmp.h
//
// Non-local goto
//
// Copyright (C) 2002 Michael Ringgaard. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.  
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.  
// 3. Neither the name of the project nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
// SUCH DAMAGE.
// 


// 2006.10.27  modified for gcc and MonaOS.
#ifndef MONASQ_SETJMP_H
#define MONASQ_SETJMP_H

typedef struct 
{
  unsigned long ebp;
  unsigned long ebx;
  unsigned long edi;
  unsigned long esi;
  unsigned long esp;
  unsigned long eip;
} monasq_jmp_buf[1];

#ifdef  __cplusplus
extern "C" {
#endif

extern int monasq_setjmp(monasq_jmp_buf env);
extern void monasq_longjmp(monasq_jmp_buf env, int value);

#define jmp_buf		monasq_jmp_buf
#define setjmp		monasq_setjmp
#define longjmp		monasq_longjmp

#ifdef  __cplusplus
}
#endif

#endif	/* MONASQ_SETJMP_H */


/*
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef SETJMP_H
#define SETJMP_H

typedef struct 
{
  unsigned long ebp;
  unsigned long ebx;
  unsigned long edi;
  unsigned long esi;
  unsigned long esp;
  unsigned long eip;
} jmp_buf[1];

#ifdef  __cplusplus
extern "C" {
#endif

int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int value);

#ifdef  __cplusplus
}
#endif


#endif
*/
