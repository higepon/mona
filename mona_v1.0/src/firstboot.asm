;-------------------------------------------------------------------------------
; Name        : firstboot.asm
; Description : boot from floppy disk, echo String and read secondboot
; Revision    : $Revision$ $Date$
; Copyright (c) 2002 HigePon, Guripon, syn
; WITHOUT ANY WARRANTY
;-------------------------------------------------------------------------------
%define DEBUG   1

;; TEMPSEC should not be 0x9FE0(on VirtualPC)
%define TEMPSEG 0x9f00
%define KERNEL  0x0100

        org     0

        jmp     short start
        nop

; fake boot parameter block(bpb) for RAWRITE
        db      "mona    "      ; OEM label (maker name and version number)
        dw      0x0200          ; bytes/sector
        db      0x01            ; sectors/cluster
        dw      0x0001          ; # of boot sectors
        db      0x02            ; # of fats
        dw      0x00e0          ; # of root directory entries
        dw      0x0b40          ; # of sectors total in image
        db      0xf0            ; media descripter: 0xf0 = 2sides18sectors
        dw      0x0009          ; # of sectors in a fat
        dw      0x0012          ; # of sectors/track
        dw      0x0002          ; # of heads
        dd      0x00000000      ; # of hidden sectors
        dd      0x00000000      ; # of sectors if > 65536
        dw      0x0000          ; drive number
        db      0x29            ; extended bios parameter block signature
        dd      0x00000000      ; volume id
        db      "NO NAME    "   ; volume label
        db      "FAT12   "      ; file system

start:
        xor     ax,ax
        mov     ds,ax
        mov     si,0x7c00       ; src = 0000:7C00 (DS:SI)
        mov     ax,TEMPSEG
        mov     es,ax
        xor     di,di           ; dst = 9FE0:0000 (ES:DI)
        mov     cx,0x0200
        ;
        rep     movsb           ; move myself
        ;
        jmp     TEMPSEG:next    ; to higher address (for kernel load)

next:
        mov     ds,ax
        mov     ax,TEMPSEG-0x1000
        mov     ss,ax
        mov     sp,0xffff
        ;
        xor     ax,ax           ; dl = boot drive (in boot default)
        int     0x13            ; boot drive initialize
        jc      $               ; it will - disk drive broken
        ;
        mov     si,boot
        call    print
        ;
        mov     ax,KERNEL
        mov     es,ax
        xor     bx,bx
        mov     ax,0x0001       ; read kernel from second sector (0 = boot sec)
        mov     di,0x0080       ; change it (now ... read 80 sectors)
        call    readsector
        ;
        mov     si,good
        call    print
        ;
        mov     dx,0x03f2       ; stop fdd motor
        mov     al,0x0c
        out     dx,al
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
        ;
        jmp     KERNEL:0000     ; jump to secondboot


; readsector:
;   ax = logical sector number (0, 1, 2 ...)
;   di = # of sectors to read
;   es:bx = pointer to read-data buffer (bx = 0, es = 0x20, 0x40, 0x60 ...)

readsector:
        pusha
.r1:    push    ax
        mov     ax,es
        neg     ax
        dec     ax
        and     ah,0x0f
        inc     ax
        mov     cl,0x05
        shr     ax,cl
        cmp     ax,di           ; ax = # of readable sectors until dma boundary
        mov     si,ax
        jbe     .dma1
        mov     si,di           ; si = # of readable sectors (end of read)
.dma1:  pop     ax
.r2:    push    ax
        mov     cx,0x0024
        xor     dx,dx
        div     cx
        xchg    dx,ax
        mov     cl,0x12
        div     cl
        sub     cl,ah
        cmp     cx,si
        jbe     .dma2
        mov     cx,si
.dma2:  mov     ch,dl           ; ch = track number
        mov     dh,al           ; dh = head number
        mov     al,cl           ; al = # of sector to read at a time
        mov     cl,ah
        inc     cl              ; cl = sector number
        xor     dl,dl           ; dl = drive number
        mov     ah,0x02
%ifdef DEBUG
        call    debug
%endif
        int     0x13
        jc      .error
        mov     dx,ax
        mov     cl,0x09
        shl     ax,cl
        add     bx,ax           ; bx = next start of buffer
        pop     ax
        add     ax,dx           ; ax = next logical sector number
        sub     di,dx
        sub     si,dx
        jnz     .r2             ; not over dma boundary
        mov     cx,es
        and     cx,0xf000
        add     cx,0x1000
        mov     es,cx
        xor     bx,bx
        or      di,di
        jnz     .r1             ; jump over dma boundary
        popa
        ret

.error:
        push    si
        mov     si,err
        call    print
        xchg    ah,al
        mov     cl,0x04
        call    tohex           ; print error code
        mov     si,crlf
        call    print
        xor     ax,ax
        xor     dl,dl
        int     0x13            ; re-initialize
        pop     si
        pop     ax
        jmp     short .r2       ; retry

debug:
        pusha
        push    cx
        mov     cl,0x0c
        call    tohex
        mov     ax,bx
        call    tohex
        pop     ax
        call    tohex
        mov     ax,dx
        call    tohex
        mov     ax,di
        call    tohex
        mov     ax,si
        call    tohex
        mov     ax,cs
        call    tohex
        mov     ax,es
        call    tohex
        mov     ax,sp
        call    tohex
        mov     si,crlf
        call    print
        popa
        ret

; print
;   ds:si = strings

print:
        pusha
        xor     bx,bx
.loop:  lodsb
        or      al,al
        jz      .print
        mov     ah,0x0e
        int     0x10
        jmp     short .loop
.print: popa
        ret

; tohex
;   ax = data to convert and display
;   cl = shift count

tohex:
        pusha
        mov     dx,ax
.loop:  shr     ax,cl
        and     al,0x0f
        cmp     al,0x0a
        jb      .low
        add     al,0x07
.low:   add     al,0x30
        mov     ah,0x0e
        int     0x10
        sub     cl,4
        mov     ax,dx
        jnc     short .loop
        mov     ax,0x0e20
        int     0x10
        popa
        ret


; various data

boot    db      "mona loading",0x0d,0x0a,0
err     db      "read error - ",0
good    db      "disk read success!",0x0d,0x0a,0
crlf    db      0x0d,0x0a,0

        times   0x01fe-($-$$) db 0
        dw      0xaa55
