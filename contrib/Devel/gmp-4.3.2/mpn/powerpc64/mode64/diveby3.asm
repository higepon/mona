dnl  PowerPC-64 mpn_divexact_by3 -- mpn by 3 exact division

dnl  Copyright 2006 Free Software Foundation, Inc.

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

C		cycles/limb
C POWER3/PPC630:     13
C POWER4/PPC970:     13
C POWER5:	     13

C INPUT PARAMETERS
define(`rp', `r3')
define(`up', `r4')
define(`n', `r5')
define(`cy', `r6')

define(`xAAAAAAAB',`r7')
define(`xAAAAAAAA',  `r9')
define(`q', `r10')
define(`ul', `r11')
define(`one',  `r12')


ASM_START()
PROLOGUE(mpn_divexact_by3c)

	mtctr	r5
	li	r7, -0x5556		C 0xFFFFFFFFFFFFAAAA
	ld	ul, 0(up)
	rldimi	r7, r7, 16, 32		C 0xFFFFFFFFAAAAAAAA
	rldimi	r7, r7, 32, 63		C 0xAAAAAAAAAAAAAAAB = 1/3

	addi	r9, r7, -1		C 0xAAAAAAAAAAAAAAAA
	li	one, 1

	subfc	ul, cy, ul		C  C = (cy <= up[0])
	subfe	cy, r1, r1		C  cy = -(cy > up[0])
	bdz	L(end)

	ALIGN(16)
L(top):	mulld	q, ul, xAAAAAAAB

	ld	ul, 8(up)
	addi	up, up, 8
	addc	r0, xAAAAAAAA, q	C set C flag if q >= 0x5555...56

	subfe	cy, cy, one		C cy = 1-cy-1+C
	subfc	r0, q, xAAAAAAAA	C set C flag if q < 0xAAAA...AA

	subfe	ul, cy, ul		C ul = ul-cy-1+C
	std	q, 0(rp)
	addi	rp, rp, 8

	subfe	cy, r1, r1
	bdnz	L(top)

L(end):	mulld	q, ul, xAAAAAAAB
	addc	r0, xAAAAAAAA, q

	subfe	cy, cy, one
	subfc	r0, q, xAAAAAAAA

	std	q, 0(rp)
	subfe	r3, r1, r1
	subf	r3, r3, cy
	blr
EPILOGUE()
