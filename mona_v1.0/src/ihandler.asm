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
[global _arch_syscall_handler]
[global _arch_cpufaulthandler_e]

[extern _cpufaultHandler_e]
[extern _arch_save_registers]
[extern _arch_set_stack_view]
[extern _MFDCHandler]
[extern _timerHandler]
[extern _keyStrokeHandler]
[extern _fault0dHandler]
[extern _syscall_entrance]
[extern _dummyHandler]

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
;        call _arch_save_registers  //for IDE Driver test
        call _timerHandler
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

;;; IRQ handler(expr)
%macro irqhandler 1
[global _arch_irqhandler_%1]
[extern _irqHandler_%1]
_arch_irqhandler_%1:
        pushad
        call _irqHandler_%1
        popad
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
        call _arch_set_stack_view
        pushad
        call _fault0dHandler
        popad
        iretd

%macro cpufaulthandler 1
[global _arch_cpufaulthandler_%1]
[extern _cpufaultHandler_%1]
_arch_cpufaulthandler_%1:
        call _arch_set_stack_view
        pushad
        call _cpufaultHandler_%1
        popad
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
        call _arch_set_stack_view
        pushad
        push dword[esp + 32]
        mov  cr2, eax
        push eax
        call _cpufaultHandler_e
        popad
        iretd

;;; entrance of syscall
_arch_syscall_handler:
        pushad
        call _arch_save_registers
        call _syscall_entrance
        popad
        iretd
