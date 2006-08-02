BITS 32
%include "macro.asm"

cglobal apm_bios_call
extern _apm_eip

section .text

_apm_bios_call:
	push ebp
	mov ebp, esp

	push ebx
	push ecx
	push edx
	push esi
	push edi

	mov eax, [cs:ebp+12]
	mov ebx, [cs:eax+4]
	mov ecx, [cs:eax+8]
	mov edx, [cs:eax+12]
	mov esi, [cs:eax+16]
	mov edi, [cs:eax+20]

	;push ebp
	;push ds
	;push es
	mov eax, 0x50
	mov ds, eax
	mov es, eax

	mov eax, [cs:ebp+8]
	mov ah, 0x53

	call far [cs:_apm_eip]
	mov ebp, 0x10
	mov ds, ebp
	mov es, ebp
	mov ebp, esp
	add ebp, 20
	jc L_apm_bios_call_err


	mov eax, [cs:ebp+12]
	mov [cs:eax], eax
	mov [cs:eax+4], ebx
	mov [cs:eax+8], ecx
	mov [cs:eax+12], edx
	mov [cs:eax+16], esi
	mov [cs:eax+20], edi
	xor eax, eax

L_apm_bios_call_err:

	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx

	pop ebp
	ret
