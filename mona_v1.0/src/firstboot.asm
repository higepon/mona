;-------------------------------------------------------------------------------
; Name        : firstboot.asm
; Description : boot from floppy disk, echo String and read secondboot
; Revision    : $Revision$ $Date$
; Copyright (c) 2002 HigePon, Guripon, syn
; WITHOUT ANY WARRANTY
;-------------------------------------------------------------------------------
%ifndef BOOT_MACRO
%define BOOT_MACRO
    %include 'boot.mac'
%endif

[ORG 0]

        jmp short start
        nop

; fake boot parameter block (bpb) for RAWRITE added by Guripon
; # Actually, fats and root dir entries are broken for trailing kernel
        db      "Mona "      ; OEM label (maker name and version number)
        dw      512             ; bytes/sector
        db      1               ; sectors/allocation unit
        dw      1               ; # of reserved sectors
        db      2               ; # of fats
        dw      0x00E0          ; # of root dir entries
        dw      2880            ; # of sectors total in image
        db      0xF0            ; media descripter: 0xf0 = 2sides18sectors
        dw      9               ; # of sectors in a fat
        dw      18              ; # of sectors/track
        dw      2               ; # of heads
        dd      0               ; # of hidden sectors
        dd      0               ; # of sectors if > 65536
        dw      0               ; drive number
        db      0x29            ; extended boot signature
        dd      0x00000000      ; volume id
        db      "NO NAME    "   ; volume label
        db      "FAT12   "      ; file system

start:
        cld
        xor ax, ax
        mov ds, ax
        mov si, 0x7c00  ; src = 0000:7C00 (DS:SI)
        mov ax, 0x9e00
        mov es, ax
        xor di, di      ; dst = 9E00:0000 (ES:DI)
        mov ecx, 0x200

        rep movsb

        jmp 0x9e00:next

next:   ; maybe 0x9E03A
        mov ds, ax
        mov es, ax
        mov ax, 0x8e00
        mov ss, ax
        mov sp, 0xFFFE

        mov ax, 0x0003          ; Clear display
        int 0x10
        PRINTSTR16 MsgNextLine
        PRINTSTR16 MsgOsStart

reset:
        PRINTSTR16 MsgResetStart
        mov ax, 0               ; reset
        mov dl, 0               ; drive 0
        int 0x13                ;
        jc  reset               ; if failed try again
        PRINTSTR16 MsgDone

        push ax

read:
        PRINTSTR16 MsgReadStart
        mov ax, 0x0100          ; 512 byte from 0040:0000 is BIOS work area
        mov es, ax              ; ... but usually it wasn't use only 256 byte
        xor bx, bx
        mov ax, 0x0001          ; read kernel from next sector
        mov di, 0x0040          ; 64 sectors
        call readsector
        PRINTSTR16 MsgDone

        jmp 0x100:0000          ; jump to the secondboot;


; readsector:
;   ax = logical sector number
;   di = # of sectors to read
;   es:bx = read-data transfer address

readsector:
        push    ax
        xor     dx,dx
        mov     si,0x0024       ; (sectors per track) * (number of heads)
        div     si
        mov     ch,al           ; ch = track number
        xchg    ax,dx
        xor     dx,dx
        mov     si,0x0012       ; number of sectors per 1 track
        div     si
        mov     dh,al           ; dh = head number
        mov     cl,dl
        inc     cl              ; cl = sector number
        mov     ax,si
        sub     al,dl           ; # of sectors to read (in a track)
        pop     si
        cmp     di,ax
        ja      .read
        mov     ax,di           ; last read
.read:  xor     dl,dl           ; dl = drive number
        mov     ah,0x02
        int     0x13
        jc      $               ; *** put some error handler! ***
        add     si,ax
        push    si
        mov     dx,ax
        mov     cl,5
        shl     dx,cl
        mov     cx,ax
        mov     ax,es
        add     ax,dx
        mov     es,ax
        pop     ax
        sub     di,cx           ; read multiple sectors
        jnz     readsector
        ret

forever:
        jmp forever

monamsg:
        MsgNextLine   db  ''                  , 0x0d, 0x0a, 0x00
        MsgOsStart    db  'Mona start...'  , 0x0d, 0x0a, 0x00
        MsgResetStart db  '  disk reset   ...', 0x00
        MsgReadStart  db  '  disk reading ...', 0x00
        MsgDone       db  'done'              , 0x0d, 0x0a, 0x00

        times 510-($-$$) db 0
        dw 0x0AA55
