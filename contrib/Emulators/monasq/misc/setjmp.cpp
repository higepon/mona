//
// setjmp.c
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
#include "misc/setjmp.h"

#ifdef  __cplusplus
extern "C" {
#endif


extern int my_setjmp(monasq_jmp_buf env);
__asm__ (
	"_my_setjmp:			\n\t"

	"mov 4(%esp),	%edx	\n\t"		// Get jmp_buf pointer
	"mov 4(%esp),	%edx	\n\t"		// Get jmp_buf pointer
	"mov 0(%esp),	%eax	\n\t"		// Save EIP
	"mov %eax, 20(%edx)		\n\t"
	"mov %ebp, 0(%edx)		\n\t"		// Save EBP, EBX, EDI, ESI, and ESP
	"mov %ebx, 4(%edx)		\n\t"
	"mov %edi, 8(%edx)		\n\t"
	"mov %esi, 12(%edx)		\n\t"
	"mov %esp, 16(%edx)		\n\t"
	"xor %eax, %eax			\n\t"		// Return 0
	"ret					\n\t"
);

extern void my_longjmp(monasq_jmp_buf env, int value);
__asm__ (
	"_my_longjmp:			\n\t"

    "mov 4(%esp),	%edx	\n\t"		// Get jmp_buf pointer
    "mov 8(%esp),	%eax	\n\t"		// Get return value (eax)

    "mov 16(%edx),	%esp	\n\t"		// Switch to new stack position
    "mov 20(%edx),	%ebx	\n\t"		// Get new EIP value and set as return address
    "mov %ebx,		0(%esp)	\n\t"
    
    "mov 0(%edx),	%ebp	\n\t"		// Restore EBP, EBX, EDI, and ESI
    "mov 4(%edx),	%ebx	\n\t"
    "mov 8(%edx),	%edi	\n\t"
    "mov 12(%edx),	%esi	\n\t"

    "ret					\n\t"
);


int monasq_setjmp(monasq_jmp_buf env) {
	return my_setjmp(env);
}

void monasq_longjmp(monasq_jmp_buf env, int value) {
	my_longjmp(env, value);
}

#ifdef  __cplusplus
}
#endif



/*
//#include "setjmp.h"

#define OFS_EBP   0
#define OFS_EBX   4
#define OFS_EDI   8
#define OFS_ESI   12
#define OFS_ESP   16
#define OFS_EIP   20


__declspec(naked) int setjmp(jmp_buf env)
{
  __asm
  {
    mov edx, 4[esp]          // Get jmp_buf pointer
    mov eax, [esp]           // Save EIP
    mov OFS_EIP[edx], eax
    mov OFS_EBP[edx], ebp    // Save EBP, EBX, EDI, ESI, and ESP
    mov OFS_EBX[edx], ebx
    mov OFS_EDI[edx], edi
    mov OFS_ESI[edx], esi
    mov OFS_ESP[edx], esp
    xor eax, eax             // Return 0
    ret
  }
}

__declspec(naked) void longjmp(jmp_buf env, int value)
{
  __asm
  {
    mov edx, 4[esp]          // Get jmp_buf pointer
    mov eax, 8[esp]          // Get return value (eax)

    mov esp, OFS_ESP[edx]    // Switch to new stack position
    mov ebx, OFS_EIP[edx]    // Get new EIP value and set as return address
    mov [esp], ebx
    
    mov ebp, OFS_EBP[edx]    // Restore EBP, EBX, EDI, and ESI
    mov ebx, OFS_EBX[edx]
    mov edi, OFS_EDI[edx]
    mov esi, OFS_ESI[edx]

    ret
  }
}
*/
