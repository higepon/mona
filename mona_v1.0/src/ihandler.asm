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
[extern _MFDCHandler]
[extern _timerHandler]

;;; fdchandler
_arch_fdchandler:
        pusha
        call _MFDCHandler
        popa
        iretd

;;; timerhandler
_arch_timerhandler:
        pusha
        call _timerHandler
        popa
        iretd
