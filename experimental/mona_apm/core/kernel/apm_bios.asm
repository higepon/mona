BITS 32
%include "macro.asm"

cglobal apm_bios_call
extern _apm_ml
extern _apm_eip
extern _apm_cs

section .text

; word apm_bios_call(word fn, apm_bios_regs *regs, FARCALL_ADDR apm_entry);
_apm_bios_call:
	push ebx
	push ecx
	push edi
	push esi
	push ebp

	mov ax, ds
	mov gs, ax
	mov ax, 0x40
	mov fs, ax

	mov eax, [esp+28]
	mov bx, [eax+2]
	mov cx, [eax+4]
	mov dx, [eax+6]
	mov si, [eax+8]
	mov di, [eax+10]

	mov eax, 0x48
	mov ds, eax
	mov es, eax
	mov eax, [esp+24]
	mov ah, 0x53

	call far [cs:_apm_eip]
	mov ebp, 0x10
	mov ds, ebp
	mov es, ebp
	jc err

	mov eax, [esp+28]
	mov [eax], ax
	mov [eax+2], bx
	mov [eax+4], cx
	mov [eax+6], dx
	mov [eax+8], si
	mov [eax+10], di
	xor eax, eax

err:
	cbw
	cwde

	pop ebp
	pop edi
	pop esi
	pop ecx
	pop ebx
	ret
