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

[extern _save_registers]
[extern _arch_set_stack_view]
[extern _MFDCHandler]
[extern _timerHandler]
[extern _keyStrokeHandler]
[extern _fault0dHandler]
[extern _syscall_handler]
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
        call _save_registers
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


;;; fault0dHandler
_arch_fault0dhandler:
        call _arch_set_stack_view
        pushad
        call _fault0dHandler
        popad
        iretd

;;; entrance of syscall
_arch_syscall_handler:
        pushad
        call _syscall_handler
        popad
        iretd
