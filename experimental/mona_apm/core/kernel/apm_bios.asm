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

	mov eax, [ebp+12]
	mov ebx, [eax+4]
	mov ecx, [eax+8]
	mov edx, [eax+12]
	mov esi, [eax+16]
	mov edi, [eax+20]

	push ebp
	push ds
	push es
	mov eax, 0x50
	mov ds, eax
	mov es, eax

	mov eax, [cs:ebp+8]
	mov ah, 0x53

	call far [cs:_apm_eip]
	;pop ebp
	;mov es, ebp
	;pop ebp
	;mov ds, ebp
	pop es
	pop ds
	pop ebp
	;jc L_apm_bios_call_err


	mov eax, [cs:ebp+12]
	mov [eax], eax
	mov [eax+4], ebx
	mov [eax+8], ecx
	mov [eax+12], edx
	mov [eax+16], esi
	mov [eax+20], edi
	xor eax, eax

L_apm_bios_call_err:

	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx

	pop ebp
	ret
