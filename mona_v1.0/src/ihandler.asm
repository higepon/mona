;;;
;      \file  ihandler.asm
;      \brief interrupt handlers

;      definition for IDT
;      Copyright (c) 2002,2003 Higepon
;      All rights reserved.
;      License=MIT/X Licnese

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
[global _arch_syscall_handler]
[global _arch_cpufaulthandler_e]

[extern _arch_switch_process]
[extern _cpufaultHandler_e]
[extern _arch_set_stack_view]
[extern _MFDCHandler]
[extern _timerHandler]
[extern _keyStrokeHandler]
[extern _fault0dHandler]
[extern _syscall_entrance]
[extern _dummyHandler]
[extern _arch_save_process_registers]
[extern _timerHandler]

[extern _g_current_process] ;; pointer to current process

%define KERNEL_DS 0x10

%macro pushAll 0
        pushad
        push ds
        push es
%endmacro

%macro popAll 0
        pop es
        pop ds
        popad
%endmacro

%macro changeData 0
        push KERNEL_DS
        pop  es
        push KERNEL_DS
        pop  ds
%endmacro


;;; fdc handler
_arch_fdchandler:
        pushAll
        changeData
        call _MFDCHandler
        popAll
        iretd

;;; timer handler
;;; save all context to Kthread* current
_arch_timerhandler:
        pushAll
        changeData
        call _arch_save_process_registers
        call _timerHandler
        popAll
        iretd

;;; keystroke handler
_arch_keystrokehandler:
        pushAll
        changeData
        xor eax, eax
        mov dx , 0x60
        in  al , dx
        push eax
        call _keyStrokeHandler
        add  esp, 0x04
        popAll
        iretd

;;; dummy handler
_arch_dummyhandler:
        pushAll
        changeData
        call _dummyHandler
        popAll
        iretd

;;; IRQ handler(expr)
%macro irqhandler 1
[global _arch_irqhandler_%1]
[extern _irqHandler_%1]
_arch_irqhandler_%1:
        pushAll
        changeData
        call _irqHandler_%1
        popAll
        iretd
%endmacro
        irqhandler 0
        irqhandler 1
        irqhandler 2
        irqhandler 3
        irqhandler 4
        irqhandler 5
        irqhandler 6
        irqhandler 7
        irqhandler 8
        irqhandler 9
        irqhandler 10
        irqhandler 11
        irqhandler 12
         irqhandler 13
        irqhandler 14
        irqhandler 15

;;; fault0dHandler
_arch_fault0dhandler:
        pushAll
        changeData
        call _fault0dHandler
        popAll
        iretd

%macro cpufaulthandler 1
[global _arch_cpufaulthandler_%1]
[extern _cpufaultHandler_%1]
_arch_cpufaulthandler_%1:
        pushAll
        changeData
        call _cpufaultHandler_%1
        popAll
        iretd
%endmacro

        cpufaulthandler 0
        cpufaulthandler 1
        cpufaulthandler 5
        cpufaulthandler 6
        cpufaulthandler 7
        cpufaulthandler 8
        cpufaulthandler a
        cpufaulthandler b
        cpufaulthandler c
;          cpufaulthandler e;;;; removed by higepon
        cpufaulthandler 10
        cpufaulthandler 11

_arch_cpufaulthandler_e:
        pushAll
        changeData
        push dword[esp + 48]
        mov  eax, cr2
        push eax
        call _cpufaultHandler_e
        add  esp, 0x08          ; remove parameter
        popAll
        add  esp, 0x04          ; remove error_cd
        iretd

;;; entrance of syscall
_arch_syscall_handler:
        pushAll
        changeData
        call _arch_save_process_registers
        call _syscall_entrance
        mov ebx, dword[_g_current_process] ; system call return code is
        mov eax, dword [ebx + 12]          ; at g_current_process->eax
        mov dword[esp + 28], eax           ; so set this to stack
        popAll
        iretd
