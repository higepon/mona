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

;;; fdc handler
_arch_fdchandler:
        pushad
        call _MFDCHandler
        popad
        iretd

;;; timer handler
_arch_timerhandler:
        pushad
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
