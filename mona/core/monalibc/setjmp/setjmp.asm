bits 32

section .bss
section .data
section .text

global __setjmp
global __longjmp

; jmp_buf:
;	eax ebx ecx edx esi edi ebp esp eip
;	0   4   8   12  16  20  24  28  32

__setjmp:	; int _setjmp(jmp_buf env);
	push	ebp
	mov	ebp, esp

	push	edi	; ediを保存
	mov	edi,	dword [ebp+8]	; envのアドレスをediに代入
	mov	dword [edi+0],	eax
	mov	dword [edi+4],	ebx
	mov	dword [edi+8],	ecx
	mov	dword [edi+12],	edx
	mov	dword [edi+16],	esi
	mov	eax,	dword [ebp-4]
	mov	dword [edi+20], eax
	mov	eax,	dword [ebp]
	mov	dword [edi+24], eax
	mov	eax, esp
	add	eax, 12
	mov	dword [edi+28], eax
	mov	eax,	dword [ebp+4]
	mov	dword [edi+32], eax
	pop	edi
	mov	eax, 0
	leave
	ret

__longjmp:	; void _longjmp(jmp_buf env, int retval);
	push	ebp
	mov	ebp, esp

	mov	edi,	dword [ebp+8]
	mov	eax,	dword [ebp+12]
	mov	dword [edi],	eax
	mov	ebp,	dword [edi+24]

	cli

	mov	esp,	dword [edi+28]
	push	dword [edi+32]
	mov	eax,	dword [edi]
	mov	ebx,	dword [edi+4]
	mov	ecx,	dword [edi+8]
	mov	edx,	dword [edi+12]
	mov	esi,	dword [edi+16]
	mov	edi,	dword [edi+20]

	sti

	ret
