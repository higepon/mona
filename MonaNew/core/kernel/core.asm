;;;
;      \file  core.asm
;      \brief core functions

;      Copyright (c) 2002,2003 Higepon
;      All rights reserved.
;      License=MIT/X License

;      \author  HigePon
;      \version $Revision$
;      \date   create:2003/03/22 update:$Date$
;;;
BITS 32
%include "macro.asm"
cglobal arch_set_stack_view
cglobal arch_save_thread_registers
cglobal arch_save_thread_registers2
cglobal arch_switch_thread_to_user1
cglobal arch_switch_thread_to_user2
cglobal arch_switch_thread_to_v861
cglobal arch_switch_thread_to_v862
cglobal arch_switch_thread1
cglobal arch_switch_thread2
cglobal arch_set_dokodemo_view
cglobal arch_idle
cglobal arch_set_cr3
cextern g_stack_view      ;; for debug stack viewer
cextern fault0dHandler
cextern g_currentThread
cextern g_tss
cextern g_dokodemo_view

section .text
arch_idle:
        hlt
        ret

;;----------------------------------------------------------------------
;; save register to current thread
;;----------------------------------------------------------------------
arch_save_thread_registers:
        mov eax, dword[g_currentThread]
        mov ebx, dword[eax + 0 ]  ; ArchThreadInfo
        fnsave [ebx + 80]
        frstor [ebx + 80]
        mov eax, dword[esp + 48]  ; get cs
        and eax, 0x03             ; check cpl is 3
        cmp eax, 0x03
        je from_user
from_kernel:
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
        mov eax, dword [esp + 24]; save esp
        add eax, 0xc
        mov dword[ebx + 28], eax
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
from_user:
        mov eax, dword[esp + 60] ; save ss3
        mov dword[ebx + 60], eax
        mov eax, dword[esp + 56] ; save esp3
        mov dword[ebx + 28], eax
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

arch_save_thread_registers2:
        mov eax, dword[g_currentThread]
        mov ebx, dword[eax + 0 ]  ; ArchThreadInfo
        fnsave [ebx + 80]
        frstor [ebx + 80]
        mov eax, dword[esp + 52]  ; get cs
        and eax, 0x03             ; check cpl is 3
        cmp eax, 0x03
        je .from_user
.from_kernel:
        mov eax, dword [esp + 48]; save eip
        mov dword[ebx], eax
        mov eax, dword [esp + 52]; save cs
        mov dword[ebx + 4], eax
        mov eax, dword [esp + 56]; save eflags
        mov dword[ebx + 8], eax
        mov eax, dword [esp + 44]; save eax
        mov dword[ebx + 12], eax
        mov eax, dword [esp + 40]; save ecx
        mov dword[ebx + 16], eax
        mov eax, dword [esp + 36]; save edx
        mov dword[ebx + 20], eax
        mov eax, dword [esp + 32]; save ebx
        mov dword[ebx + 24], eax
        mov eax, dword [esp + 28]; save esp
        add eax, 0xc
        mov dword[ebx + 28], eax
        mov eax, dword [esp + 24]; save ebp
        mov dword[ebx + 32], eax
        mov eax, dword [esp + 20]; save esi
        mov dword[ebx + 36], eax
        mov eax, dword [esp + 16]; save edi
        mov dword[ebx + 40], eax
        mov eax, [esp + 12]      ; save ds
        mov dword[ebx + 44], eax
        mov eax, [esp + 8]      ; save es
        mov dword[ebx + 48], eax
        ret
.from_user:
        mov eax, dword[esp + 64] ; save ss3
        mov dword[ebx + 60], eax
        mov eax, dword[esp + 60] ; save esp3
        mov dword[ebx + 28], eax
        mov eax, dword [esp + 48]; save eip
        mov dword[ebx], eax
        mov eax, dword [esp + 52]; save cs
        mov dword[ebx + 4], eax
        mov eax, dword [esp + 56]; save eflags
        mov dword[ebx + 8], eax
        mov eax, dword [esp + 44]; save eax
        mov dword[ebx + 12], eax
        mov eax, dword [esp + 40]; save ecx
        mov dword[ebx + 16], eax
        mov eax, dword [esp + 36]; save edx
        mov dword[ebx + 20], eax
        mov eax, dword [esp + 42]; save ebx
        mov dword[ebx + 24], eax
        mov eax, dword [esp + 24]; save ebp
        mov dword[ebx + 32], eax
        mov eax, dword [esp + 20]; save esi
        mov dword[ebx + 36], eax
        mov eax, dword [esp + 16]; save edi
        mov dword[ebx + 40], eax
        mov eax, [esp + 12]      ; save ds
        mov dword[ebx + 44], eax
        mov eax, [esp + 8]      ; save es
        mov dword[ebx + 48], eax
        ret

;;----------------------------------------------------------------------
;; swtich thread to user without page change
;;----------------------------------------------------------------------
arch_switch_thread_to_user1:
        mov eax, dword[g_currentThread]
        mov ebx, dword[eax + 0 ]     ; ArchThreadInfo
        frstor [ebx + 80]
        mov ecx, dword[g_tss]        ; tss
        mov eax, dword[ebx + 68]     ; get esp0
        mov dword[ecx + 4], eax      ; restore esp0
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
        pop  ebx                     ; restore ebx
        iretd                        ; switch to next

;;----------------------------------------------------------------------
;; swtich thread to user and change page
;;----------------------------------------------------------------------
arch_switch_thread_to_user2:
        mov eax, dword[g_currentThread]
        mov ebx, dword[eax + 0 ]     ; ArchThreadInfo
        frstor [ebx + 80]
        mov ecx, dword[g_tss]        ; tss
        mov eax, dword[ebx + 68]     ; get esp0
        mov dword[ecx + 4], eax      ; restore esp0
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
        frstor [ebx + 80]
        mov ecx, dword[g_tss]        ; tss
        mov eax, dword[ebx + 68]     ; get esp0
        mov dword[ecx + 4], eax      ; restore esp0
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
        pop  ebx                     ; restore ebx
        iretd                        ; switch to next

;;----------------------------------------------------------------------
;; switch thread and change page
;;----------------------------------------------------------------------
arch_switch_thread2:
        mov eax, dword[g_currentThread]
        mov ebx, dword[eax + 0 ]     ; ArchThreadInfo
        frstor [ebx + 80]
        mov ecx, dword[g_tss]        ; tss
        mov eax, dword[ebx + 68]     ; get esp0
        mov dword[ecx + 4], eax      ; restore esp0
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
        pop  ebx                     ; restore ebx
        iretd                        ; switch to next

;;----------------------------------------------------------------------
;; swtich thread to v86 without page change
;;----------------------------------------------------------------------
arch_switch_thread_to_v861:
        mov eax, dword[g_currentThread]
        mov ebx, dword[eax + 0 ]     ; ArchThreadInfo
        frstor [ebx + 80]
        mov ecx, dword[g_tss]        ; tss
        mov eax, dword[ebx + 68]     ; get esp0
        mov dword[ecx + 1], eax      ; restore esp0
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
        push dword[ebx + 56]         ; gs
        push dword[ebx + 52]         ; fs
        push dword[ebx + 44]         ; ds
        push dword[ebx + 48]         ; es
        push dword[ebx + 60]         ; push ss  here dpl lowwer
        push dword[ebx + 28]         ; push esp here dpl lowwer
        push dword[ebx + 8]          ; push eflags
        push dword[ebx + 4]          ; push cs
        push dword[ebx + 0]          ; push eip
        push dword[ebx + 24]
        pop  ebx                     ; restore ebp
        iretd                        ; switch to next

;;----------------------------------------------------------------------
;; swtich thread to v86 and change page
;;----------------------------------------------------------------------
arch_switch_thread_to_v862:
        mov eax, dword[g_currentThread]
        mov ebx, dword[eax + 0 ]     ; ArchThreadInfo
        frstor [ebx + 80]
        mov ecx, dword[g_tss]        ; tss
        mov eax, dword[ebx + 68]     ; get esp0
        mov dword[ecx + 1], eax      ; restore esp0
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
        push dword[ebx + 56]         ; gs
        push dword[ebx + 52]         ; fs
        push dword[ebx + 44]         ; ds
        push dword[ebx + 48]         ; es
        push dword[ebx + 60]         ; push ss  here dpl lowwer
        push dword[ebx + 28]         ; push esp here dpl lowwer
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

arch_set_dokodemo_view:
        push  eax
        push  ebx
        push  eax
        mov   eax, g_dokodemo_view
        mov   ebx, cs
        mov   dword[eax + 32 ], ebx ; CS
        mov   ebx, ds
        mov   dword[eax + 36 ], ebx ; DS
        mov   ebx, ss
        mov   dword[eax + 40], ebx ; SS
        mov   ebx, cr3
        mov   dword[eax + 44], ebx ; CR3
        pushf
        mov   ebx, dword[esp + 0]
        mov   dword[eax + 48], ebx ; EFLAGS
        popf
        pop   ebx
        mov   dword[eax + 0], ebx   ; EAX
        pop   ebx
        mov   dword[eax + 4], ebx   ; EBX
        mov   dword[eax + 8], ecx   ; ECX
        mov   dword[eax + 12], edx   ; EDX
        mov   dword[eax + 16], esp   ; ESP
        mov   dword[eax + 20], ebp   ; EBP
        mov   dword[eax + 24], esi   ; ESI
        mov   dword[eax + 28], edi   ; EDI
        pop   eax
        ret
