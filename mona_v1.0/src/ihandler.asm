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
[global _arch_fault0dhandler]
[global _arch_timerhandler]
[global _arch_keystrokehandler]
[global _arch_dummyhandler]
[global _arch_kthread_switch]

[extern _MFDCHandler]
[extern _timerHandler]
[extern _keyStrokeHandler]
[extern _fault0dHandler]
[extern _dummyHandler]
[extern _g_kthread_current];; pointer to current thread
[extern _g_stack_view];; pointer to current thread

;;; fdc handler
_arch_fdchandler:
        pushad
        call _MFDCHandler
        popad
        iretd

;;; timer handler
;;; save all context to Kthread* current
_arch_timerhandler:
;          call _arch_set_stack_view
        pushad
        call _save_registers
        call _timerHandler
        popad
        iretd

;;; this fuction saves all registers to current thread struct
;;; should be called by interrupt handlers
_save_registers:
        mov ebx, dword[_g_kthread_current]
        mov eax, dword [esp + 36]; save eip
        mov dword[ebx], eax
        mov eax, dword [esp + 40]; save cs
        mov dword[ebx + 4], eax
        mov eax, dword [esp + 44]; save eflags
        mov dword[ebx + 8], eax
        mov eax, dword [esp + 32]; save eax
        mov dword[ebx + 12], eax
        mov eax, dword [esp + 28]; save ecx
        mov dword[ebx + 16], eax
        mov eax, dword [esp + 24]; save edx
        mov dword[ebx + 20], eax
        mov eax, dword [esp + 20]; save ebx
        mov dword[ebx + 24], eax
        mov eax, dword [esp + 16]; save esp
        mov dword[ebx + 28], eax
        mov eax, dword [esp + 12]; save ebp
        mov dword[ebx + 32], eax
        mov eax, dword [esp +  8]; save esi
        mov dword[ebx + 36], eax
        mov eax, dword [esp +  4]; save edi
        mov dword[ebx + 40], eax
        ret

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

;;; kthread switch to next
_arch_kthread_switch:
        mov ebx, dword[_g_kthread_current]
        mov eax, dword[ebx + 12]     ; restore eax
        mov ecx, dword[ebx + 16]     ; restore ecx
        mov edx, dword[ebx + 20]     ; restore edx
        mov esp, dword[ebx + 28]     ; restore esp
        mov ebp, dword[ebx + 32]     ; restore ebp
        mov esi, dword[ebx + 36]     ; restore esi
        mov edi, dword[ebx + 40]     ; restore edi
        push 0x200    ; push eflags
;          push dword [ebx + 8]    ; push eflags
        push dword[ebx + 4]    ; push cs
        push dword[ebx + 0]    ; push eip
        push dword[ebx + 24]
        pop  ebx                ; restore ebp
;          call _arch_set_stack_view
        iretd                   ; switch to next


;;; fault0dHandler
_arch_fault0dhandler:
        call _arch_set_stack_view
        pushad
        call _fault0dHandler
        popad
        iretd

_arch_set_stack_view:
        push eax
        push ebx
        mov ebx, _g_stack_view
        mov eax, dword [esp + 12]
        mov [ebx], eax
        mov eax, dword [esp + 16]
        mov [ebx + 4], eax
        mov eax, dword [esp + 20]
        mov [ebx + 8], eax
        mov eax, dword [esp + 24]
        mov [ebx + 12], eax
        mov eax, dword [esp + 28]
        mov [ebx + 16], eax
        mov eax, dword [esp + 32]
        mov [ebx + 20], eax
        mov eax, dword [esp + 36]
        mov [ebx + 24], eax
        mov eax, dword [esp + 40]
        mov [ebx + 28], eax
        pop ebx
        pop eax
        ret
