;;;
;      \file  core.asm
;      \brief core functions

;      Copyright (c) 2002,2003 Higepon
;      All rights reserved.
;      License=MIT/X Licnese

;      \author  HigePon
;      \version $Revision$
;      \date   create:2003/03/22 update:$Date$
;;;
BITS 32
%include "macro.asm"
cglobal arch_set_stack_view
cglobal arch_save_thread_registers
cglobal arch_switch_thread_to_user1
cglobal arch_switch_thread_to_user2
cglobal arch_switch_thread1
cglobal arch_switch_thread2
cextern g_stack_view      ;; for debug stack viewer
cextern fault0dHandler
cextern g_currentThread

;;----------------------------------------------------------------------
;; save register to current thread
;;----------------------------------------------------------------------
arch_save_thread_registers:
        mov eax, dword[g_currentThread]
        mov ebx, dword[eax + 0 ]  ; ArchThreadInfo
        mov eax, dword[esp + 48]  ; get cs
        and eax, 0x03             ; check cpl is 3
        cmp eax, 0x03
        jnz dpl00
        mov eax, dword[esp + 60] ; save ss3
        mov dword[ebx + 60], eax
        mov eax, dword[esp + 56] ; save esp3
        mov dword[ebx + 28], eax
dpl00:
        mov eax, dword [esp + 44]; save eip
        mov dword[ebx], eax
        mov eax, dword [esp + 48]; save cs
        mov dword[ebx + 4], eax
        mov eax, dword [esp + 52]; save eflags
        mov dword[ebx + 8], eax
        mov eax, dword [esp + 40]; save eax
        mov dword[ebx + 12], eax
        mov eax, dword [esp + 36]; save ecx
        mov dword[ebx + 16], eax
        mov eax, dword [esp + 32]; save edx
        mov dword[ebx + 20], eax
        mov eax, dword [esp + 28]; save ebx
        mov dword[ebx + 24], eax
        jz  dpl03
        mov eax, dword [esp + 24]; save esp
        add eax, 0xc
        mov dword[ebx + 28], eax
dpl03:
        mov eax, dword [esp + 20]; save ebp
        mov dword[ebx + 32], eax
        mov eax, dword [esp + 16]; save esi
        mov dword[ebx + 36], eax
        mov eax, dword [esp + 12]; save edi
        mov dword[ebx + 40], eax
        mov eax, [esp + 8]      ; save ds
        mov dword[ebx + 44], eax
        mov eax, [esp + 4]      ; save es
        mov dword[ebx + 48], eax
        ret

;;----------------------------------------------------------------------
;; swtich thread to user without page change
;;----------------------------------------------------------------------
arch_switch_thread_to_user1:
        mov eax, dword[g_currentThread]
        mov ebx, dword[eax + 0 ]     ; ArchThreadInfo
        mov eax, dword[ebx + 12]     ; restore eax
        mov ecx, dword[ebx + 16]     ; restore ecx
        mov edx, dword[ebx + 20]     ; restore edx
        mov esp, dword[ebx + 28]     ; restore esp
        mov ebp, dword[ebx + 32]     ; restore ebp
        mov esi, dword[ebx + 36]     ; restore esi
        mov edi, dword[ebx + 40]     ; restore edi
        mov es , word[ebx + 48]      ; restore es
        mov ds , word[ebx + 44]      ; restore ds
        push dword[ebx + 60]         ; push ss  here dpl lowwer
        push dword[ebx + 28]         ; push esp here dpl lowwer
        push dword[ebx + 8]          ; push eflags
        push dword[ebx + 4]          ; push cs
        push dword[ebx + 0]          ; push eip
        push dword[ebx + 24]
        pop  ebx                     ; restore ebp
        iretd                        ; switch to next

;;----------------------------------------------------------------------
;; swtich thread to user and change page
;;----------------------------------------------------------------------
arch_switch_thread_to_user2:
        mov eax, dword[g_currentThread]
        mov ebx, dword[eax + 0 ]     ; ArchThreadInfo
        mov eax, dword[ebx + 76]     ; page directory
        mov cr3, eax                 ; change page directory
        mov eax, dword[ebx + 12]     ; restore eax
        mov ecx, dword[ebx + 16]     ; restore ecx
        mov edx, dword[ebx + 20]     ; restore edx
        mov esp, dword[ebx + 28]     ; restore esp
        mov ebp, dword[ebx + 32]     ; restore ebp
        mov esi, dword[ebx + 36]     ; restore esi
        mov edi, dword[ebx + 40]     ; restore edi
        mov es , word[ebx + 48]      ; restore es
        mov ds , word[ebx + 44]      ; restore ds
        push dword[ebx + 60]         ; push ss  here dpl lowwer
        push dword[ebx + 28]         ; push esp here dpl lowwer
        push dword[ebx + 8]          ; push eflags
        push dword[ebx + 4]          ; push cs
        push dword[ebx + 0]          ; push eip
        push dword[ebx + 24]
        pop  ebx                     ; restore ebp
        iretd                        ; switch to next

;;----------------------------------------------------------------------
;; switch thread without page change
;;----------------------------------------------------------------------
arch_switch_thread1:
        mov eax, dword[g_currentThread]
        mov ebx, dword[eax + 0 ]     ; ArchThreadInfo
        mov eax, dword[ebx + 12]     ; restore eax
        mov ecx, dword[ebx + 16]     ; restore ecx
        mov edx, dword[ebx + 20]     ; restore edx
        mov esp, dword[ebx + 28]     ; restore esp
        mov ebp, dword[ebx + 32]     ; restore ebp
        mov esi, dword[ebx + 36]     ; restore esi
        mov edi, dword[ebx + 40]     ; restore edi
        mov es , word[ebx + 48]      ; restore es
        mov ds , word[ebx + 44]      ; restore ds
        push dword[ebx + 8]          ; push eflags
        push dword[ebx + 4]          ; push cs
        push dword[ebx + 0]          ; push eip
        push dword[ebx + 24]
        pop  ebx                     ; restore ebp
        iretd                        ; switch to next

;;----------------------------------------------------------------------
;; switch thread and change page
;;----------------------------------------------------------------------
arch_switch_thread2:
        mov eax, dword[g_currentThread]
        mov ebx, dword[eax + 0 ]     ; ArchThreadInfo
        mov eax, dword[ebx + 76]     ; page directory
        mov cr3, eax                 ; change page directory
        mov eax, dword[ebx + 12]     ; restore eax
        mov ecx, dword[ebx + 16]     ; restore ecx
        mov edx, dword[ebx + 20]     ; restore edx
        mov esp, dword[ebx + 28]     ; restore esp
        mov ebp, dword[ebx + 32]     ; restore ebp
        mov esi, dword[ebx + 36]     ; restore esi
        mov edi, dword[ebx + 40]     ; restore edi
        mov es , word[ebx + 48]      ; restore es
        mov ds , word[ebx + 44]      ; restore ds
        push dword[ebx + 8]          ; push eflags
        push dword[ebx + 4]          ; push cs
        push dword[ebx + 0]          ; push eip
        push dword[ebx + 24]
        pop  ebx                     ; restore ebp
        iretd                        ; switch to next

arch_set_stack_view:
        push eax
        push ebx
        mov ebx, g_stack_view
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
