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
