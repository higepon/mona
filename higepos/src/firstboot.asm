;-------------------------------------------------------------------------------
; Name        : firstboot.asm
; Description : boot from floppy disk, echo String and read secondboot
; Revision    : $Revision$ $Date$
; Copyright (c) 2002 HigePon
; WITHOUT ANY WARRANTY
;-------------------------------------------------------------------------------
%ifndef BOOT_MACRO
%define BOOT_MACRO
    %include 'boot.mac'
%endif

[ORG 0]

; use segment 0x07C0 and offset is address of start
; cs = 0x07C0
jmp 0x07C0:start

start:
        cli                     ; disable interrupt
        mov ax, cs              ; code segment address is 0x07C0
        mov ds, ax              ; when data access, use 0x07C0 segment
        mov ss, ax              ; when use stack, user 0x07C0 segment
        sti                     ; enable interrupt
        mov al, 3               ; Clear display
        mov ah, 0               ;
        int 0x10                ;
        PRINTSTR16 MsgNextLine
        PRINTSTR16 MsgOsStart

reset:
        PRINTSTR16 MsgResetStart
        mov ax, 0               ; reset
        mov dl, 0               ; drive 0
        int 0x13                ;
        jc  reset               ; if failed try again
        PRINTSTR16 MsgDone

read:
        PRINTSTR16 MsgReadStart
        mov ax, 0x100
        mov es, ax              ; es:bx is
        mov bx, 0               ; data buffer

        mov ah, 0x02            ; read sectors into memory
        mov al, 18              ; number of sectors to read
        mov ch, 0               ; low eight bits of cylinder number
        mov cl, 2               ; sector number
        mov dh, 0               ; head number
        mov dl, 0               ; drive number
        int 13h                 ; read!
        jc  read
        PRINTSTR16 MsgDone

        jmp 0x100:0000          ; jump to the secondboot

higeposmsg:
        MsgNextLine   db  ''                  , 0x0d, 0x0a, 0x00
        MsgOsStart    db  'Higepos start...'  , 0x0d, 0x0a, 0x00
        MsgResetStart db  '  disk reset   ...', 0x00
        MsgReadStart  db  '  disk reading ...', 0x00
        MsgDone       db  'done'              , 0x0d, 0x0a, 0x00

forever:
        jmp forever

        times 510-($-$$) db 0
        dw 0x0AA55
