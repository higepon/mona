;;;
;      \file  ihandler.asm
;      \brief interrupt handlers

;      definition for IDT
;      Copyright (c) 2003 HigePon
;      WITHOUT ANY WARRANTY

;      \author  HigePon
;      \version $Revision$
;      \date   create:2003/02/24 update:$Date$
;;;
BITS 32

[global _arch_fdchandler]
[global _arch_timerhandler]
[global _arch_keystrokehandler]
[global _arch_dummyhandler]

[extern _MFDCHandler]
[extern _timerHandler]
[extern _keyStrokeHandler]
[extern _dummyHandler]
[extern _current];; pointer to current thread

;;; fdc handler
_arch_fdchandler:
        pushad
        call _MFDCHandler
        popad
        iretd

;;; timer handler
;;; save all context to Kthread* current
_arch_timerhandler:
        pushad
        mov ebx, [_current]
        mov eax, dword [esp + 32]; save eip
        mov [ebx], eax
        mov eax, dword [esp + 36]; save cs
        mov [ebx + 4], eax
        mov eax, dword [esp + 40]; save eflags
        mov [ebx + 8], eax
        mov eax, dword [esp + 28]; save eax
        mov [ebx + 12], eax
        mov eax, dword [esp + 24]; save ecx
        mov [ebx + 16], eax
        mov eax, dword [esp + 20]; save edx
        mov [ebx + 20], eax
        mov eax, dword [esp + 16]; save ebx
        mov [ebx + 24], eax
        mov eax, dword [esp + 12]; save esp
        mov [ebx + 28], eax
        mov eax, dword [esp +  8]; save ebp
        mov [ebx + 32], eax
        mov eax, dword [esp +  4]; save esi
        mov [ebx + 36], eax
        mov eax, dword [esp +  0]; save edi
        mov [ebx + 40], eax
        call _dummyHandler
        popad
        iretd

;;; keystroke handler
_arch_keystrokehandler:
        pushad
        call _keyStrokeHandler
        popad
        iretd

;;; dummy handler
_arch_dummyhandler:
        pushad
        call _dummyHandler
        popad
        iretd
