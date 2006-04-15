bits 32
%include "macro.asm"

cexport _alloca

section .text

_alloca:
	pop	edx
	sub	esp,	eax
	jmp	edx
