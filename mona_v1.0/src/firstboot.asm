;-------------------------------------------------------------------------------
; Name        : firstboot.asm
; Description : boot from floppy disk, echo String and read secondboot
;               We use FAT12 File System.
; Revision    : $Revision$ $Date$
; Copyright (c) 2002,2003 Guripon, HigePon
; WITHOUT ANY WARRANTY
;-------------------------------------------------------------------------------
%define kernel  0x1000
%define tempseg 0x9f00
%define fat     0x6000

;%define DEBUG  1

        org     0

        jmp     short realstart
        nop

; boot parameter block(bpb)
oem     db      "mona    "
bps     dw      0x0200
spc     db      0x01
nob     dw      0x0001
nof     db      0x02
rde     dw      0x00e0
nos     dw      0x0b40
med     db      0xf0
spf     dw      0x0009
spt     dw      0x0012
noh     dw      0x0002
        dd      0x00000000
        dd      0x00000000
        dw      0x0000
        db      0x29
        dd      0x00000000
        db      "NO NAME    "
        db      "FAT12   "

realstart:
        xor     si,si
        mov     di,si
        mov     ax,0x07c0
        mov     ds,ax
        mov     ax,tempseg
        mov     es,ax
        mov     cx,0x0100
        rep     movsw
        jmp     tempseg:realnext
realnext:
        mov     ds,ax
        sub     ah,0x10
        mov     ss,ax
        xor     sp,sp
        ;
        xor     ax,ax
        int     0x13
        jc      $
        ;
        mov     si,boot
        call    putstring
        ;
        xor     dx,dx
        mov     cx,dx
        mov     bx,dx
        mov     ax,0x9000
        mov     es,ax
        mov     ax,word [spf]
        mov     cl,byte [nof]
        mul     cx
        inc     ax
        mov     di,word [rde]
        push    di
        shr     di,4            ; rde / 16 = required sectors
        inc     di
        call    readsector

        xor     di,di
        pop     bx
kernel_next:
        mov     si,bname
        mov     cx,0x000b
        push    di
        rep     cmpsb
        pop     di
        je      kernel_found
        add     di,0x20
%ifdef DEBUG
       call    register_dump
%endif
        dec     bx
        jnz     kernel_next
        jmp     file_not_found
kernel_found:
        add     di,0x1a                 ; start sector
        mov     cx,[es:di]
%ifdef DEBUG
       call    register_dump
%endif
        mov     bx,fat
        mov     ax,0x0001
        mov     di,[spf]
        call    readsector
        mov     ax,es
        mov     ds,ax
        xor     ax,ax
        mov     es,ax
        mov     bx,0x1000
kernel_load:
        mov     ax,cx
        add     ax,31
        mov     di,1
        call    readsector
        push    bx
        mov     bx,cx
        call    get_fat
        pop     bx
%ifdef DEBUG
       call    register_dump
%endif
        cmp     ax,0x0fff
        je      end_of_kernel
        mov     cx,ax
        add     bx,0x0200
        jnc     kernel_load
        mov     bx,es
        add     bh,0x10
        mov     es,bx
        xor     bx,bx
        jmp     kernel_load
end_of_kernel:
        xor     ax,ax
        mov     ds,ax
        mov     es,ax
        mov     ss,ax
        mov     sp,kernel
        call    memory_dump

        jmp     0x0100:0x0000

get_fat:
        mov     si,bx
        shr     si,1
        mov     ax,[si+bx+fat]
        jnc     _get_fat
        shr     ax,4
_get_fat:
        and     ah,0x0f
        ret

file_not_found:
%ifdef DEBUG
       call    register_dump
%endif
;          mov     si,not_found
;          call    putstring
        jmp     $               ; loop forever

; readsector
;   ax = start sector, di = number of sectors to read
;   es:bx = read address (es = 64kb align, bx = 512 bytes align)

readsector:
        pusha
        push    es
        ;
_read0: mov     si,bx
        neg     si
        dec     si
        shr     si,9
        inc     si
        cmp     si,di
        jbe     _read1
        mov     si,di           ; di < si
_read1: push    ax
        xor     dx,dx
        mov     cx,0x0024
        div     cx              ; ax = track number
        xchg    dx,ax
        mov     cl,0x12
        div     cl              ; ah = sector number, al = head number
        sub     cl,ah
        cmp     cx,si
        jbe     .read1
        mov     cx,si           ; si < cx
.read1: mov     ch,dl           ; ch = track number
        mov     dh,al           ; dh = head number
        mov     al,cl           ; al = number of sectors to read
        mov     cl,ah           ; cl = sector number
        inc     cl
        xor     dl,dl           ; dl = drive number
        mov     ah,0x02
%ifdef DEBUG
        call    register_dump
%endif
        int     0x13
        jc      _read_error
        mov     dx,ax
        mov     cl,0x09
        shl     ax,cl
        add     bx,ax
        pop     ax
        add     ax,dx
        sub     di,dx
        sub     si,dx
        jnz     _read1
        mov     cx,es
        add     ch,0x10
        mov     es,cx
        xor     bx,bx
        or      di,di
        jnz     _read0
        ;
        pop     es
        popa
        ret

_read_error:
%ifdef DEBUG
       call    register_dump
%endif
        xor     ax,ax
        xor     dl,dl
        int     0x13
        pop     ax
        jmp     _read1

putstring:
        pusha
.put:   cs lodsb
        or      al,al
        jz      .pute
        xor     bx,bx
        mov     ah,0x0e
        int     0x10
        jmp     .put
.pute:  popa
        ret

; tohex
;   ax = data to convert and display, cl = shift count

tohex:
        pusha
.tol:   rol     ax,4
        push    ax
        and     al,0x0f
        cmp     al,0x0a
        jb      .to
        add     al,0x07
.to:    add     al,0x30
        mov     ah,0x0e
        int     0x10
        pop     ax
        dec     cl
        jnz     .tol
        mov     ax,0x0e20
        int     0x10
        popa
        ret

memory_dump:
        pusha
        mov     bl,0x10
.mem:   mov     ah,byte [es:di]
        inc     di
        mov     cl,0x02
        call    tohex
        dec     bl
        jnz     .mem
        mov     si,crlf
        call    putstring
        popa
        ret
%ifdef DEBUG
register_dump:
        pusha
        push    sp
        push    es
        push    ds
        push    cs
        push    di
        push    si
        push    dx
        push    cx
        push    bx
        push    ax
        mov     cx,0x0a04
.reg:   pop     ax
        call    tohex
        dec     ch
        jnz     .reg
        mov     si,crlf
        call    putstring
        popa
        ret
%endif

bname   db      "KERNEL  IMG"
boot    db      "Mona loading..."
;  not_found db "KERNEL.IMG not found"
crlf    db      0x0d,0x0a,0

        times   0x01fe-($-$$) db 0
        dw      0xaa55
