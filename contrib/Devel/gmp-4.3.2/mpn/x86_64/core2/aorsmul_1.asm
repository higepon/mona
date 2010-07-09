dnl  x86-64 mpn_addmul_1 and mpn_submul_1, optimized for "Core 2".

dnl  Copyright 2003, 2004, 2005, 2007, 2008 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 3 of the License, or (at
dnl  your option) any later version.

dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')

C	     cycles/limb
C K8,K9:	 4
C K10:		 4
C P4:		 ?
C P6-15:	 4.3-4.7 (fluctuating)

C INPUT PARAMETERS
define(`rp',	`%rdi')
define(`up',	`%rsi')
define(`n',	`%rdx')
define(`v0',	`%rcx')

ifdef(`OPERATION_addmul_1',`
      define(`ADDSUB',        `add')
      define(`func',  `mpn_addmul_1')
')
ifdef(`OPERATION_submul_1',`
      define(`ADDSUB',        `sub')
      define(`func',  `mpn_submul_1')
')

MULFUNC_PROLOGUE(mpn_addmul_1 mpn_submul_1)

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(func)
	push	%r15
	push	%r12
	push	%r13
	lea	(%rdx), %r15
	neg	%r15

	mov	(up), %rax

	bt	$0, %r15
	jc	L(odd)

	lea	(rp,%rdx,8), rp
	lea	(up,%rdx,8), up
	mul	%rcx

	lea	(%rax), %r11
	mov	8(up,%r15,8), %rax
	mov	(rp,%r15,8), %r13
	lea	(%rdx), %r12

	add	$2, %r15
	jns	L(n2)

	mul	%rcx
	lea	(%rax), %r8
	mov	(up,%r15,8), %rax
	mov	-8(rp,%r15,8), %r10
	lea	(%rdx), %r9
	jmp	L(m)

L(odd):	lea	(rp,%rdx,8), rp
	lea	(up,%rdx,8), up
	mul	%rcx
	add	$1, %r15
	jns	L(n1)

L(gt1):	lea	(%rax), %r8
	mov	(up,%r15,8), %rax
	mov	-8(rp,%r15,8), %r10
	lea	(%rdx), %r9
	mul	%rcx
	lea	(%rax), %r11
	mov	8(up,%r15,8), %rax
	mov	(rp,%r15,8), %r13
	lea	(%rdx), %r12
	add	$2, %r15
	jns	L(end)

	ALIGN(16)
L(top):	mul	%rcx
	ADDSUB	%r8, %r10
	lea	(%rax), %r8
	mov	0(up,%r15,8), %rax
	adc	%r9, %r11
	mov	%r10, -24(rp,%r15,8)
	mov	-8(rp,%r15,8), %r10
	lea	(%rdx), %r9
	adc	$0, %r12
L(m):	mul	%rcx
	ADDSUB	%r11, %r13
	lea	(%rax), %r11
	mov	8(up,%r15,8), %rax
	adc	%r12, %r8
	mov	%r13, -16(rp,%r15,8)
	mov	0(rp,%r15,8), %r13
	lea	(%rdx), %r12
	adc	$0, %r9

	add	$2, %r15
	js	L(top)

L(end):	mul	%rcx
	ADDSUB	%r8, %r10
	adc	%r9, %r11
	mov	%r10, -24(rp,%r15,8)
	mov	-8(rp,%r15,8), %r10
	adc	$0, %r12
L(r):	ADDSUB	%r11, %r13
	adc	%r12, %rax
	mov	%r13, -16(rp,%r15,8)
	adc	$0, %rdx
L(x):	ADDSUB	%rax, %r10
	mov	%r10, -8(rp,%r15,8)
	mov	$0, %eax
	adc	%rdx, %rax
L(ret):	pop	%r13
	pop	%r12
	pop	%r15
	ret
L(n2):	mul	%rcx
	mov	-8(rp,%r15,8), %r10
	jmp	L(r)
L(n1):	mov	-8(rp,%r15,8), %r10
	jmp	L(x)
EPILOGUE()
