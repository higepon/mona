dnl  AMD64 mpn_sqr_basecase.

dnl  Contributed to the GNU project by Torbjorn Granlund.

dnl  Copyright 2008, 2009 Free Software Foundation, Inc.

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

C The inner loops of this code are the result of running a code generation and
C optimization tool suite written by David Harvey and Torbjorn Granlund.

C NOTES
C   * This code only handles operands up to SQR_KARATSUBA_THRESHOLD_MAX.  That
C     means we can safely use 32-bit operations for all sizes, unlike in e.g.,
C     mpn_addmul_1.
C   * The jump table could probably be optimized, at least for non-pic.
C   * The special code for n=1,2,3 was quickly written.  It is probably too
C     large and unnecessarily slow.
C   * Consider combining small cases code so that the n=k-1 code jumps into
C     the middle of the n=k code.
C   * Avoid saving registers for small cases code.
C   * Needed variables:
C    n   r11  input size
C    i   r8   work left, initially n
C    j   r9   inner loop count
C        r15  unused
C    v0  r13
C    v1  r14
C    rp  rdi
C    up  rsi
C    w0  rbx
C    w1  rcx
C    w2  rbp
C    w3  r10
C    tp  r12
C    lo  rax
C    hi  rdx
C        rsp

C INPUT PARAMETERS
define(`rp',	  `%rdi')
define(`up',	  `%rsi')
define(`n_param', `%rdx')

C We should really trim this, for better spatial locality.  Alternatively,
C we could grab the upper part of the stack area, leaving the lower part
C instead of the upper part unused.
define(`SQR_KARATSUBA_THRESHOLD_MAX', 120)
define(`STACK_ALLOC', eval(8*2*SQR_KARATSUBA_THRESHOLD_MAX))

define(`n',	`%r11')
define(`tp',	`%r12')
define(`i',	`%r8')
define(`j',	`%r9')
define(`v0',	`%r13')
define(`v1',	`%r14')
define(`w0',	`%rbx')
define(`w1',	`%rcx')
define(`w2',	`%rbp')
define(`w3',	`%r10')

define(`SPECIAL_CODE_FOR_4',1)


ASM_START()
	TEXT
	ALIGN(16)

PROLOGUE(mpn_sqr_basecase)
	add	$-48, %rsp
	mov	%rbx, 40(%rsp)
	mov	%rbp, 32(%rsp)
	mov	%r12, 24(%rsp)
	mov	%r13, 16(%rsp)
	mov	%r14, 8(%rsp)

	mov	R32(n_param), R32(n)		C free original n register (rdx)
	mov	R32(n_param), R32(%rcx)
	and	$3, R32(%rcx)
	lea	4(%rcx), %rbx
	cmp	$4, R32(n_param)
	cmovg	%rbx, %rcx
	lea	L(jmptab)(%rip), %rax
	jmp	*(%rax,%rcx,8)
	JUMPTABSECT
	ALIGN(8)
L(jmptab):
	.quad	L(4)
	.quad	L(1)
	.quad	L(2)
	.quad	L(3)
	.quad	L(0m4)
	.quad	L(1m4)
	.quad	L(2m4)
	.quad	L(3m4)
	TEXT

L(1):	mov	(up), %rax
	mul	%rax
	mov	%rax, (rp)
	mov	%rdx, 8(rp)
	add	$40, %rsp
	pop	%rbx
	ret

L(2):	mov	(up), %rax
	mul	%rax
	mov	%rax, (rp)
	mov	%rdx, %r9
	mov	8(up), %rax
	mul	%rax
	mov	%rax, %r10
	mov	%rdx, %r11
	mov	8(up), %rax
	mov	(up), %rbx
	mul	%rbx
	add	%rax, %r9
	adc	%rdx, %r10
	adc	$0, %r11
	add	%rax, %r9
	mov	%r9, 8(rp)
	adc	%rdx, %r10
	mov	%r10, 16(rp)
	adc	$0, %r11
	mov	%r11, 24(rp)
	add	$40, %rsp
	pop	%rbx
	ret

L(3):	mov	(up), %rax
	mul	%rax
	mov	%rax, (rp)
	mov	%rdx, 8(rp)
	mov	8(up), %rax
	mul	%rax
	mov	%rax, 16(rp)
	mov	%rdx, 24(rp)
	mov	16(up), %rax
	mul	%rax
	mov	%rax, 32(rp)
	mov	%rdx, 40(rp)

	mov	(up), %rbx
	mov	8(up), %rax
	mul	%rbx
	mov	%rax, %r8
	mov	%rdx, %r9
	mov	16(up), %rax
	mul	%rbx
	xor	R32(%r10), R32(%r10)
	add	%rax, %r9
	adc	%rdx, %r10

	mov	8(up), %rbx
	mov	16(up), %rax
	mul	%rbx
	xor	R32(%r11), R32(%r11)
	add	%rax, %r10
	adc	%rdx, %r11
	add	%r8, %r8
	adc	%r9, %r9
	adc	%r10, %r10
	adc	%r11, %r11
	mov	$0, R32(%rbx)
	adc	%rbx, %rbx
	add	%r8, 8(rp)
	adc	%r9, 16(rp)
	adc	%r10, 24(rp)
	adc	%r11, 32(rp)
	adc	%rbx, 40(rp)
	add	$40, %rsp
	pop	%rbx
	ret

ifdef(`SPECIAL_CODE_FOR_4',`
L(4):	mov	(up), %rax
	mul	%rax
	mov	%rax, (rp)
	mov	%rdx, 8(rp)
	mov	8(up), %rax
	mul	%rax
	mov	%rax, 16(rp)
	mov	%rdx, 24(rp)
	mov	16(up), %rax
	mul	%rax
	mov	%rax, 32(rp)
	mov	%rdx, 40(rp)
	mov	24(up), %rax
	mul	%rax
	mov	%rax, 48(rp)
	mov	%rdx, 56(rp)

	mov	(up), %rbx
	mov	8(up), %rax
	mul	%rbx
	mov	%rax, %r8
	mov	%rdx, %r9
	mov	16(up), %rax
	mul	%rbx
	xor	R32(%r10), R32(%r10)
	add	%rax, %r9
	adc	%rdx, %r10
	mov	24(up), %rax
	mul	%rbx
	xor	R32(%r11), R32(%r11)
	add	%rax, %r10
	adc	%rdx, %r11
	mov	8(up), %rbx
	mov	16(up), %rax
	mul	%rbx
	xor	R32(%r12), R32(%r12)
	add	%rax, %r10
	adc	%rdx, %r11
	adc	$0, %r12
	mov	24(up), %rax
	mul	%rbx
	add	%rax, %r11
	adc	%rdx, %r12
	mov	16(up), %rbx
	mov	24(up), %rax
	mul	%rbx
	xor	R32(%rbp), R32(%rbp)
	add	%rax, %r12
	adc	%rdx, %rbp

	add	%r8, %r8
	adc	%r9, %r9
	adc	%r10, %r10
	adc	%r11, %r11
	adc	%r12, %r12
	mov	$0, R32(%rbx)
	adc	%rbp, %rbp

	adc	%rbx, %rbx
	add	%r8, 8(rp)
	adc	%r9, 16(rp)
	adc	%r10, 24(rp)
	adc	%r11, 32(rp)
	adc	%r12, 40(rp)
	adc	%rbp, 48(rp)
	adc	%rbx, 56(rp)
	add	$24, %rsp
	pop	%r12
	pop	%rbp
	pop	%rbx
	ret
')

L(0m4):	add	$-STACK_ALLOC, %rsp
	lea	(%rsp,n,8), tp		C point tp in middle of result operand
	lea	(up,n,8), up		C point up at end of input operand

	lea	-1(n), i
C Function mpn_mul_1_m3(tp, up - i, i, up[-i - 1])
	mov	$-1, j
	sub	i, j

	lea	-24(tp), tp		C offset FIXME

	mov	(up,j,8), v0
	mov	8(up,j,8), %rax
	mul	v0
	xor	R32(w2), R32(w2)
	mov	%rax, w0
	mov	16(up,j,8), %rax
	mov	%rdx, w3
	jmp	L(L3)

	ALIGN(16)
L(mul_1_m3_top):
	add	%rax, w2
	mov	w3, (tp,j,8)
	mov	(up,j,8), %rax
	adc	%rdx, w1
	xor	R32(w0), R32(w0)
	mul	v0
	xor	R32(w3), R32(w3)
	mov	w2, 8(tp,j,8)
	add	%rax, w1
	adc	%rdx, w0
	mov	8(up,j,8), %rax
	mov	w1, 16(tp,j,8)
	xor	R32(w2), R32(w2)
	mul	v0
	add	%rax, w0
	mov	16(up,j,8), %rax
	adc	%rdx, w3
L(L3):	xor	R32(w1), R32(w1)
	mul	v0
	add	%rax, w3
	mov	24(up,j,8), %rax
	adc	%rdx, w2
	mov	w0, 24(tp,j,8)
	mul	v0
	add	$4, j
	js	L(mul_1_m3_top)

	add	%rax, w2
	mov	w3, (tp)
	adc	%rdx, w1
	mov	w2, 8(tp)
	mov	w1, 16(tp)
	lea	eval(24+2*8)(tp), tp	C tp += 2, undo offset FIXME
ifdef(`SPECIAL_CODE_FOR_4',`',`
	cmp	$3, R32(i)
	je	L(last)
')
	jmp	L(dowhile)

L(1m4):	add	$-STACK_ALLOC, %rsp
	lea	(%rsp,n,8), tp		C point tp in middle of result operand
	lea	(up,n,8), up		C point up at end of input operand

	lea	(n), i
C Function mpn_mul_2s_m0(tp, up - i, i, up - i - 1)
	mov	$3, R32(j)
	sub	i, j

	lea	8(up), up		C offset FIXME

	mov	-32(up,j,8), v0		C u0
	mov	-24(up,j,8), v1		C u1
	mov	-24(up,j,8), %rax	C u1
	mul	v0			C u0 * u1
	mov	%rdx, w1
	xor	R32(w2), R32(w2)
	mov	%rax, -24(tp,j,8)
	jmp	L(m0)

	ALIGN(16)
L(mul_2_m0_top):
	mul	v1
	add	%rax, w0
	adc	%rdx, w1
	mov	-24(up,j,8), %rax
	mov	$0, R32(w2)
	mul	v0
	add	%rax, w0
	mov	-24(up,j,8), %rax
	adc	%rdx, w1
	adc	$0, R32(w2)
	mul	v1			C v1 * u0
	add	%rax, w1
	mov	w0, -24(tp,j,8)
	adc	%rdx, w2
L(m0):	mov	-16(up,j,8), %rax	C u2, u6 ...
	mul	v0			C u0 * u2
	mov	$0, R32(w3)
	add	%rax, w1
	adc	%rdx, w2
	mov	-16(up,j,8), %rax
	adc	$0, R32(w3)
	mov	$0, R32(w0)
	mov	w1, -16(tp,j,8)
	mul	v1
	add	%rax, w2
	mov	-8(up,j,8), %rax
	adc	%rdx, w3
	mov	$0, R32(w1)
	mul	v0
	add	%rax, w2
	mov	-8(up,j,8), %rax
	adc	%rdx, w3
	adc	$0, R32(w0)
	mul	v1
	add	%rax, w3
	mov	w2, -8(tp,j,8)
	adc	%rdx, w0
	mov	(up,j,8), %rax
	mul	v0
	add	%rax, w3
	adc	%rdx, w0
	adc	$0, R32(w1)
	add	$4, j
	mov	-32(up,j,8), %rax
	mov	w3, -32(tp,j,8)
	js	L(mul_2_m0_top)

	mul	v1
	add	%rax, w0
	adc	%rdx, w1
	mov	w0, -8(tp)
	mov	w1, (tp)

	lea	-8(up), up		C undo offset FIXME
	lea	eval(3*8)(tp), tp	C tp += 3
	add	$-2, R32(i)		C i -= 2
	cmp	$3, R32(i)
	je	L(last)
	jmp	L(dowhile)



L(2m4):	add	$-STACK_ALLOC, %rsp
	lea	(%rsp,n,8), tp		C point tp in middle of result operand
	lea	(up,n,8), up		C point up at end of input operand

	lea	-1(n), i
C Function mpn_mul_1_m1(tp, up - (i - 1), i - 1, up[-i])
	mov	$1, R32(j)
	sub	i, j

	lea	-24(tp), tp		C offset FIXME

	mov	-16(up,j,8), v0
	mov	-8(up,j,8), %rax
	mul	v0
	mov	%rax, w2
	mov	(up,j,8), %rax
	mov	%rdx, w1
	jmp	L(L1)

	ALIGN(16)
L(mul_1_m1_top):
	add	%rax, w2
	mov	w3, (tp,j,8)
	mov	(up,j,8), %rax
	adc	%rdx, w1
L(L1):	xor	R32(w0), R32(w0)
	mul	v0
	xor	R32(w3), R32(w3)
	mov	w2, 8(tp,j,8)
	add	%rax, w1
	adc	%rdx, w0
	mov	8(up,j,8), %rax
	mov	w1, 16(tp,j,8)
	xor	R32(w2), R32(w2)
	mul	v0
	add	%rax, w0
	mov	16(up,j,8), %rax
	adc	%rdx, w3
	xor	R32(w1), R32(w1)
	mul	v0
	add	%rax, w3
	mov	24(up,j,8), %rax
	adc	%rdx, w2
	mov	w0, 24(tp,j,8)
	mul	v0
	add	$4, j
	js	L(mul_1_m1_top)

	add	%rax, w2
	mov	w3, (tp)
	adc	%rdx, w1
	mov	w2, 8(tp)
	mov	w1, 16(tp)

	lea	eval(24+2*8)(tp), tp	C tp += 2, undo offset FIXME
	jmp	L(dowhile_mid)



L(3m4):	add	$-STACK_ALLOC, %rsp
	lea	(%rsp,n,8), tp		C point tp in middle of result operand
	lea	(up,n,8), up		C point up at end of input operand

	lea	(n), i
C Function mpn_mul_2s_m2(tp, up - i + 1, i - 1, up - i)
	mov	$1, R32(j)
	sub	i, j

	lea	8(up), up		C offset FIXME

	mov	-16(up,j,8), v0
	mov	-8(up,j,8), v1
	mov	-8(up,j,8), %rax
	mul	v0			C v0 * u0
	mov	%rdx, w3
	xor	R32(w0), R32(w0)
	xor	R32(w1), R32(w1)
	mov	%rax, -8(tp,j,8)
	jmp	L(m2)

	ALIGN(16)
L(mul_2_m2_top):
	mul	v1
	add	%rax, w0
	adc	%rdx, w1
	mov	-24(up,j,8), %rax
	mov	$0, R32(w2)
	mul	v0
	add	%rax, w0
	mov	-24(up,j,8), %rax
	adc	%rdx, w1
	adc	$0, R32(w2)
	mul	v1			C v1 * u0
	add	%rax, w1
	mov	w0, -24(tp,j,8)
	adc	%rdx, w2
	mov	-16(up,j,8), %rax
	mul	v0
	mov	$0, R32(w3)
	add	%rax, w1
	adc	%rdx, w2
	mov	-16(up,j,8), %rax
	adc	$0, R32(w3)
	mov	$0, R32(w0)
	mov	w1, -16(tp,j,8)
	mul	v1
	add	%rax, w2
	mov	-8(up,j,8), %rax
	adc	%rdx, w3
	mov	$0, R32(w1)
	mul	v0
	add	%rax, w2
	mov	-8(up,j,8), %rax
	adc	%rdx, w3
	adc	$0, R32(w0)
	mul	v1
	add	%rax, w3
	mov	w2, -8(tp,j,8)
	adc	%rdx, w0
L(m2):	mov	(up,j,8), %rax
	mul	v0
	add	%rax, w3
	adc	%rdx, w0
	adc	$0, R32(w1)
	add	$4, j
	mov	-32(up,j,8), %rax
	mov	w3, -32(tp,j,8)
	js	L(mul_2_m2_top)

	mul	v1
	add	%rax, w0
	adc	%rdx, w1
	mov	w0, -8(tp)
	mov	w1, (tp)

	lea	-8(up), up		C undo offset FIXME
	lea	eval(3*8)(tp), tp	C tp += 3
	add	$-2, R32(i)		C i -= 2
	jmp	L(dowhile_mid)

L(dowhile):
C Function mpn_addmul_2s_m2(tp, up - (i - 1), i - 1, up - i)
	mov	$-1, j
	sub	i, j

	lea	-24(tp), tp		C offset FIXME
	lea	-8(up), up		C offset FIXME

	mov	16(up,j,8), v0
	mov	24(up,j,8), v1
	mov	24(up,j,8), %rax
	mul	v0
	xor	R32(w3), R32(w3)
	add	%rax, 24(tp,j,8)
	adc	%rdx, w3
	xor	R32(w0), R32(w0)
	xor	R32(w1), R32(w1)
	jmp	L(am2)

	ALIGN(16)
L(addmul_2_m2_top):
	add	w3, (tp,j,8)
	adc	%rax, w0
	mov	8(up,j,8), %rax
	adc	%rdx, w1
	mov	$0, R32(w2)
	mul	v0
	add	%rax, w0
	mov	8(up,j,8), %rax
	adc	%rdx, w1
	adc	$0, R32(w2)
	mul	v1				C v1 * u0
	add	w0, 8(tp,j,8)
	adc	%rax, w1
	adc	%rdx, w2
	mov	16(up,j,8), %rax
	mov	$0, R32(w3)
	mul	v0				C v0 * u1
	add	%rax, w1
	mov	16(up,j,8), %rax
	adc	%rdx, w2
	adc	$0, R32(w3)
	mul	v1				C v1 * u1
	add	w1, 16(tp,j,8)
	adc	%rax, w2
	mov	24(up,j,8), %rax
	adc	%rdx, w3
	mul	v0
	mov	$0, R32(w0)
	add	%rax, w2
	adc	%rdx, w3
	mov	$0, R32(w1)
	mov	24(up,j,8), %rax
	adc	$0, R32(w0)
	mul	v1
	add	w2, 24(tp,j,8)
	adc	%rax, w3
	adc	%rdx, w0
L(am2):	mov	32(up,j,8), %rax
	mul	v0
	add	%rax, w3
	mov	32(up,j,8), %rax
	adc	%rdx, w0
	adc	$0, R32(w1)
	mul	v1
	add	$4, j
	js	L(addmul_2_m2_top)

	add	w3, (tp)
	adc	%rax, w0
	adc	%rdx, w1
	mov	w0, 8(tp)
	mov	w1, 16(tp)

	lea	eval(2*8)(tp), tp	C tp += 2
	add	$-2, R32(i)		C i -= 2

	lea	24(tp), tp		C undo offset FIXME
	lea	8(up), up		C undo offset FIXME

L(dowhile_mid):
C Function mpn_addmul_2s_m0(tp, up - (i - 1), i - 1, up - i)
	mov	$1, R32(j)
	sub	i, j

	lea	-24(tp), tp		C offset FIXME
	lea	-8(up), up		C offset FIXME

	mov	(up,j,8), v0
	mov	8(up,j,8), v1
	mov	8(up,j,8), %rax
	mul	v0
	xor	R32(w1), R32(w1)
	add	%rax, 8(tp,j,8)
	adc	%rdx, w1
	xor	R32(w2), R32(w2)
	jmp	L(20)

	ALIGN(16)
L(addmul_2_m0_top):
	add	w3, (tp,j,8)
	adc	%rax, w0
	mov	8(up,j,8), %rax
	adc	%rdx, w1
	mov	$0, R32(w2)
	mul	v0
	add	%rax, w0
	mov	8(up,j,8), %rax
	adc	%rdx, w1
	adc	$0, R32(w2)
	mul	v1				C v1 * u0
	add	w0, 8(tp,j,8)
	adc	%rax, w1
	adc	%rdx, w2
L(20):	mov	16(up,j,8), %rax
	mov	$0, R32(w3)
	mul	v0				C v0 * u1
	add	%rax, w1
	mov	16(up,j,8), %rax
	adc	%rdx, w2
	adc	$0, R32(w3)
	mul	v1				C v1 * u1
	add	w1, 16(tp,j,8)
	adc	%rax, w2
	mov	24(up,j,8), %rax
	adc	%rdx, w3
	mul	v0
	mov	$0, R32(w0)
	add	%rax, w2
	adc	%rdx, w3
	mov	$0, R32(w1)
	mov	24(up,j,8), %rax
	adc	$0, R32(w0)
	mul	v1
	add	w2, 24(tp,j,8)
	adc	%rax, w3
	adc	%rdx, w0
	mov	32(up,j,8), %rax
	mul	v0
	add	%rax, w3
	mov	32(up,j,8), %rax
	adc	%rdx, w0
	adc	$0, R32(w1)
	mul	v1
	add	$4, j
	js	L(addmul_2_m0_top)

	add	w3, (tp)
	adc	%rax, w0
	adc	%rdx, w1
	mov	w0, 8(tp)
	mov	w1, 16(tp)

	lea	24(tp), tp		C undo offset FIXME
	lea	8(up), up		C undo offset FIXME

	lea	eval(2*8)(tp), tp	C tp += 2
	add	$-2, R32(i)		C i -= 2

	cmp	$3, R32(i)
	jne	L(dowhile)

L(last):

C Function mpn_addmul_2s_2
	mov	-24(up), v0
	mov	-16(up), v1
	mov	-16(up), %rax
	mul	v0
	xor	R32(w3), R32(w3)
	add	%rax, -32(tp)
	adc	%rdx, w3
	xor	R32(w0), R32(w0)
	xor	R32(w1), R32(w1)
	mov	-8(up), %rax
	mul	v0
	add	%rax, w3
	mov	-8(up), %rax
	adc	%rdx, w0
	mul	v1
	add	w3, -24(tp)
	adc	%rax, w0
	adc	%rdx, w1
	mov	w0, -16(tp)
	mov	w1, -8(tp)

C Function mpn_sqr_diag_addlsh1
	mov	R32(n), R32(j)
	shl	$3, n
	sub	n, up

	mov	(%rsp), %r11

	bt	$0, j
	lea	-4(j,j),j
	jc	L(odd)

L(evn):	lea	(rp,j,8), rp
	lea	(up,j,4), up
	lea	8(%rsp,j,8), tp
	neg	j

	add	%r11, %r11
	sbb	R32(%rbx), R32(%rbx)		C save CF
	mov	(up,j,4), %rax
	mul	%rax
	add	%rdx, %r11
	mov	%rax, (rp,j,8)
	jmp	L(d0)

L(odd):	lea	-16(rp,j,8), rp
	lea	-8(up,j,4), up
	lea	-8(%rsp,j,8), tp
	neg	j

	add	%r11, %r11
	sbb	R32(%rbp), R32(%rbp)		C save CF
	mov	8(up,j,4), %rax
	mul	%rax
	add	%rdx, %r11
	mov	%rax, 16(rp,j,8)
	jmp	L(d1)

	ALIGN(16)
L(top):	mov	(up,j,4), %rax
	mul	%rax
	add	R32(%rbp), R32(%rbp)		C restore carry
	adc	%rax, %r10
	adc	%rdx, %r11
	mov	%r10, (rp,j,8)
L(d0):	mov	%r11, 8(rp,j,8)
	mov	(tp,j,8), %r10
	adc	%r10, %r10
	mov	8(tp,j,8), %r11
	adc	%r11, %r11
	nop
	sbb	R32(%rbp), R32(%rbp)		C save CF
	mov	8(up,j,4), %rax
	mul	%rax
	add	R32(%rbx), R32(%rbx)		C restore carry
	adc	%rax, %r10
	adc	%rdx, %r11
	mov	%r10, 16(rp,j,8)
L(d1):	mov	%r11, 24(rp,j,8)
	mov	16(tp,j,8), %r10
	adc	%r10, %r10
	mov	24(tp,j,8), %r11
	adc	%r11, %r11
	sbb	R32(%rbx), R32(%rbx)		C save CF
	add	$4, j
	js	L(top)

L(end):	mov	(up,j,4), %rax
	mul	%rax
	add	R32(%rbp), R32(%rbp)		C restore carry
	adc	%rax, %r10
	adc	%rdx, %r11
	mov	%r10, (rp,j,8)
	mov	%r11, 8(rp,j,8)
	mov	(tp,j,8), %r10
	adc	%r10, %r10
	sbb	R32(%rbp), R32(%rbp)		C save CF
	neg	R32(%rbp)
	mov	8(up,j,4), %rax
	mul	%rax
	add	R32(%rbx), R32(%rbx)		C restore carry
	adc	%rax, %r10
	adc	%rbp, %rdx
	mov	%r10, 16(rp,j,8)
	mov	%rdx, 24(rp,j,8)

	add	$eval(8+STACK_ALLOC), %rsp
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	ret
EPILOGUE()
