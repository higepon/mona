dnl  AMD64 mpn_divexact_1 -- mpn by limb exact division.

dnl  Copyright 2001, 2002, 2004, 2005, 2006 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 3 of the License, or (at
dnl  your option) any later version.

dnl The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')


C	     cycles/limb
C K8,K9:	10
C K10:		10
C P4:		33
C P6-15 (Core2):13.25
C P6-28 (Atom):	42

C A quick adoption of the 32-bit K7 code.


C INPUT PARAMETERS
C rp		rdi
C up		rsi
C n		rdx
C divisor	rcx

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_divexact_1)
	pushq	%rbx

	movq	%rcx, %rax
	movl	$0, %ecx		C shift count
	movq	%rdx, %r8

	btl	$0, %eax
	jnc	L(evn)			C skip bsfq unless divisor is even

L(odd):	movq	%rax, %rbx
	shrl	%eax
	andl	$127, %eax		C d/2, 7 bits

ifdef(`PIC',`
	movq	binvert_limb_table@GOTPCREL(%rip), %rdx
',`
	movabsq	$binvert_limb_table, %rdx
')

	movzbl	(%rax,%rdx), %eax	C inv 8 bits

	movq	%rbx, %r11		C d without twos

	leal	(%rax,%rax), %edx	C 2*inv
	imull	%eax, %eax		C inv*inv
	imull	%ebx, %eax		C inv*inv*d
	subl	%eax, %edx		C inv = 2*inv - inv*inv*d, 16 bits

	leal	(%rdx,%rdx), %eax	C 2*inv
	imull	%edx, %edx		C inv*inv
	imull	%ebx, %edx		C inv*inv*d
	subl	%edx, %eax		C inv = 2*inv - inv*inv*d, 32 bits

	leaq	(%rax,%rax), %rdx	C 2*inv
	imulq	%rax, %rax		C inv*inv
	imulq	%rbx, %rax		C inv*inv*d
	subq	%rax, %rdx		C inv = 2*inv - inv*inv*d, 64 bits

	leaq	(%rsi,%r8,8), %rsi	C up end
	leaq	-8(%rdi,%r8,8), %rdi	C rp end
	negq	%r8			C -n

	movq	%rdx, %r10		C final inverse
	movq	(%rsi,%r8,8), %rax	C up[0]

	incq	%r8
	jz	L(one)

	movq	(%rsi,%r8,8), %rdx	C up[1]

	shrdq	%cl, %rdx, %rax

	xorl	%ebx, %ebx
	jmp	L(entry)

L(evn):	bsfq	%rax, %rcx
	shrq	%cl, %rax
	jmp	L(odd)

	ALIGN(8)
L(top):
	C rax	q
	C rbx	carry bit, 0 or 1
	C rcx	shift
	C rdx
	C rsi	up end
	C rdi	rp end
	C r8	counter, limbs, negative

	mulq	%r11			C carry limb in rdx

	movq	-8(%rsi,%r8,8), %rax
	movq	(%rsi,%r8,8), %r9

	shrdq	%cl, %r9, %rax
	nop

	subq	%rbx, %rax		C apply carry bit
	setc	%bl

	subq	%rdx, %rax		C apply carry limb
	adcq	$0, %rbx

L(entry):
	imulq	%r10, %rax

	movq	%rax, (%rdi,%r8,8)
	incq	%r8
	jnz	L(top)


	mulq	%r11			C carry limb in rdx

	movq	-8(%rsi), %rax		C up high limb
	shrq	%cl, %rax

	subq	%rbx, %rax		C apply carry bit

	subq	%rdx, %rax		C apply carry limb

	imulq	%r10, %rax

	movq	%rax, (%rdi)

	popq	%rbx
	ret


L(one):
	shrq	%cl, %rax

	imulq	%r10, %rax

	movq	%rax, (%rdi)

	popq	%rbx
	ret

EPILOGUE()
