;;;
;      \file  core.asm
;      \brief core functions

;      Copyright (c) 2003 HigePon
;      WITHOUT ANY WARRANTY

;      \author  HigePon
;      \version $Revision$
;      \date   create:2003/03/22 update:$Date$
;;;
BITS 32

[global _arch_set_stack_view]
[global _arch_save_process_registers]
[global _arch_switch_process]
[global _arch_switch_process_to_user_mode]

[extern _g_current_process] ;; pointer to current process
[extern _g_stack_view]      ;; for debug stack viewer

[extern _fault0dHandler]

_arch_save_process_registers:
        mov ebx, dword[_g_current_process]
        mov eax, dword [esp + 40] ; get cs
        and  eax, 0x03            ; check dpl is 3
        cmp eax, 0x03
        jnz dpl0
        mov eax, dword[esp + 52] ; save ss3
        mov dword[ebx + 48], eax
        mov eax, dword[esp + 48] ; save esp3
        mov dword[ebx + 28], eax
dpl0:
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
        jz  dpl3
        mov eax, dword [esp + 16]; save esp
        add eax, 0xc
        mov dword[ebx + 28], eax
dpl3:
        mov eax, dword [esp + 12]; save ebp
        mov dword[ebx + 32], eax
        mov eax, dword [esp +  8]; save esi
        mov dword[ebx + 36], eax
        mov eax, dword [esp +  4]; save edi
        mov dword[ebx + 40], eax
        mov eax, ds              ; save ds
        mov dword[ebx + 44], eax
;         mov eax, ss              ; save ss
;         mov dword[ebx + 48], eax
        ret

_arch_switch_process:
        mov ebx, dword[_g_current_process]
        mov eax, dword[ebx + 12]     ; restore eax
        mov ecx, dword[ebx + 16]     ; restore ecx
        mov edx, dword[ebx + 20]     ; restore edx
        mov esp, dword[ebx + 28]     ; restore esp
        mov ebp, dword[ebx + 32]     ; restore ebp
        mov esi, dword[ebx + 36]     ; restore esi
        mov edi, dword[ebx + 40]     ; restore edi
        mov ds , word[ebx + 44]      ; restore ds
        push dword[ebx + 8]          ; push eflags
        push dword[ebx + 4]          ; push cs
        push dword[ebx + 0]          ; push eip
        push dword[ebx + 24]
        pop  ebx                     ; restore ebp
        iretd                        ; switch to next

_arch_switch_process_to_user_mode:
        mov ebx, dword[_g_current_process]
        mov eax, dword[ebx + 12]     ; restore eax
        mov ecx, dword[ebx + 16]     ; restore ecx
        mov edx, dword[ebx + 20]     ; restore edx
        mov esp, dword[ebx + 28]     ; restore esp
        mov ebp, dword[ebx + 32]     ; restore ebp
        mov esi, dword[ebx + 36]     ; restore esi
        mov edi, dword[ebx + 40]     ; restore edi
        mov ds , word[ebx + 44]      ; restore ds
        mov es , word[ebx + 44]      ; restore es
        mov fs , word[ebx + 44]      ; restore fs
        mov gs , word[ebx + 44]      ; restore gs
        push dword[ebx + 48]         ; push ss  here dpl lowwer
        push dword[ebx + 28]         ; push esp here dpl lowwer
        push dword[ebx + 8]          ; push eflags
        push dword[ebx + 4]          ; push cs
        push dword[ebx + 0]          ; push eip
        push dword[ebx + 24]
        pop  ebx                     ; restore ebp
        iretd                        ; switch to next

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
