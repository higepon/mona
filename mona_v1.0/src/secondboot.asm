;-------------------------------------------------------------------------------
; Name        : secondboot.asm
; Description : firstboot read this from disk and execute.
; Revision    : $Revision$ $Date$
; Copyright (c) 2002 HigePon
; WITHOUT ANY WARRANTY
;-------------------------------------------------------------------------------
[bits 16]

a20enable:
        in      al,0x64
        test    al,0x02
        jnz     a20enable
        cli
        mov     al,0xD1
        out     0x64,al
        mov     al,0xDF
        out     0x60,al
        sti
        ;
graphicalmode:
        mov ax, 0x0012
        int 0x10
RealToProtect:
        mov  ax, cs             ; we jump from firstboot
        mov  ds, ax             ; so ds is changed
        lgdt [gdtr]             ; load gdtr
        cli                     ; disable interrupt
        mov  eax, cr0           ; real
        or   eax, 1             ; to
        mov  cr0, eax           ; protect
        jmp  flush_q1

;-------------------------------------------------------------------------------
; GDT definition: It is temporary.
;-------------------------------------------------------------------------------
gdtr:
        dw gdt_end - gdt0 - 1   ; gdt limit
        dd gdt0 + 0x100 * 16    ; start adderess

gdt0:                           ; segment 00
        dw 0                    ; segment limitL
        dw 0                    ; segment baseL
        db 0                    ; segment baseM
        db 0                    ; segment type
        db 0                    ; segment limitH
        db 0                    ; segment baseH

gdt08:                          ; segment 08(code segment)
        dw 0xffff               ; segment limitL
        dw 0                    ; segment baseL
        db 0                    ; segment baseM
        db 0x9a                 ; Type Code
        db 0xff               ; segment limitH
        db 0                    ; segment baseH

gdt10:                          ; segment 10(data segment)
        dw 0xffff               ; segment limitL
        dw 0                    ; segment baseL
        db 0                    ; segment baseM
        db 0x92                 ; Type Data
        db 0xff                 ; segment limitH
        db 0                    ; segment baseH

gdt18:                          ; segment 18(stack segment)
        dw 0                    ; segment limitL
        dw 0                    ; segment baseL
        db 0                    ; segment baseM
        db 0x96                 ; Type Stack
        db 0xC0                 ; segment limitH
        db 0                    ; segment baseH

gdt_end:                        ; end of gdt

[bits 32]
flush_q1:
        db 0eah
        dw set_cs_desc1 + 0x100 * 16
        dw 08h

set_cs_desc1:
        mov  ax, 0x10           ; ds & es
        mov  ds, ax             ; selector is
        mov  es, ax             ; 0x10
        mov  ax, 0x18           ; ss selector
        mov  ss, ax             ; is 0x18
        mov  esp, 1024*1024*3   ; sp is 8MB
        push eax
        jmp  0x200
;  hang:
;          jmp hang


        times 512-($-$$) db 0
