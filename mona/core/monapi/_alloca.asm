bits 32
%include "macro.asm"

section .text

;;  N.B. export directive should be placed after the section text
;;         Otherwise nasm 2.07 will SEGV.
cexport _alloca

_alloca:
    pop edx
    sub esp,    eax
    jmp edx
