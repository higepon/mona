;-------------------------------------------------------------------------------
; Name        : secondboot.asm
; Description : firstboot read this from disk and execute.
; Revision    : $Revision$ $Date$
; Copyright (c) 2002,2003 and 2004 Higepon
; All rights reserved.
; License=MIT/X Licnese
;-------------------------------------------------------------------------------
%define vesa_info        0x800
%define vesa_info_detail 0x830

[bits 16]
        cli
a20enable:
        in      al,0x64
        test    al,0x02
        jnz     a20enable

        mov     al,0xD1
        out     0x64,al
a20enable_1:
        in      al,0x64
        test    al,0x02
        jnz     a20enable_1    ;wait every KBC cmd.

        mov     al,0xDF
        out     0x60,al
a20enable_2:
        in      al,0x64
        test    al,0x02
        jnz     a20enable_2

        mov     al,0xFF
        out     0x64,al
a20enable_3:
        in      al,0x64
        test    al,0x02
        jnz     a20enable_3

        call vesa_mode

;-------------------------------------------------------------------------------
; To Protect mode
;-------------------------------------------------------------------------------
RealToProtect:
;;; Real to Protect
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

;-------------------------------------------------------------------------------
; try VESA mode
;------------------------------------------------------------------------------
vesa_mode:
        mov ax, word[cs:vesares]
        mov dx, 0x010f
        cmp ax, 320+1
        jc vesa_loop
        add dx, 3
        cmp ax, 640+1
        jc vesa_loop
        add dx, 3
        cmp ax, 800+1
        jc vesa_loop
        add dx, 3
        cmp ax, 1024+1
        jc vesa_loop
        add dx, 3
        cmp ax, 1280+1
        jc vesa_loop
        add dx, 3
vesa_loop:
        mov ah, byte[cs:vesabpp]
        cmp ah, 16+1
        jc vesa_16bpp
        call try_vesa_mode
        cmp ax, 0
        je vesa_mode_ret
vesa_16bpp:
        dec dx
        call try_vesa_mode
        cmp ax, 0
        je vesa_mode_ret
        cmp dx, 0x010e
        je vesa_not_supported
        sub dx, 2
        jmp vesa_loop
vesa_not_supported:
        mov  di, vesa_info
        mov  byte[di], 'N'
        jmp  graphicalmode
graphicalmode:
        mov ax, 0x0012
        int 0x10
vesa_mode_ret:
        ret

;; try VESA dx = mode number
try_vesa_mode:
        mov  ax, 0x4F02         ; functon 02h
        mov  bx, dx             ; set video mode
        or   bx, 0x4000         ; set linear mode
        int  0x10
        cmp  ax, 0x004F
        jne  vesa_this_mode_ng
get_vesa_info:
        xor bx, bx
        mov es, bx
        mov ax, 0x4F00            ; function 00h
        mov di, vesa_info         ; 0x0000:vesa_info
        int 0x10
        cmp ax, 0x004F
        jne vesa_this_mode_ng
get_vesa_info_detail
        mov ax, 0x4F01            ; function 01h
        mov cx, dx                ; set video mode
        mov di, vesa_info_detail  ; 0x0000:vesa_info_detail
        int 0x10
        cmp ax, 0x004F
        jne vesa_this_mode_ng
vesa_this_mode_ok:
        xor ax, ax
        ret
vesa_this_mode_ng:
        mov ax, 0x01
        ret

; set_vesa_palette:
;         push bp
;         mov bp, sp
;         sub sp, 6               ; word i; byte r, g, b, a
;         mov word[bp - 1], 0     ; i
;         mov byte[bp - 3], 7     ; r
;         mov byte[bp - 4], 7     ; g
;         mov byte[bp - 5], 7     ; b
;         mov byte[bp - 6], 0xff  ; alpha
;         mov ax, 0x4f09          ; function settings
;         xor bx, bx              ;     set palette
;         mov cx, 1               ;     number of entries to change
;         lea di, [bp - 3]        ;     r, g, b, a array
; r_loop:
; g_loop:
; b_loop:
;         mov dx, [bp - 1]
;         int 0x10
;         add byte[bp - 5], 8
;         cmp byte[bp - 5], 255
;         inc word[bp - 1]
;         jne b_loop
;         mov byte[bp - 5], 7
;         add byte[bp - 4], 8
;         cmp byte[bp - 4], 255
;         inc word[bp - 1]
;         jne g_loop
;         mov byte[bp - 4], 7
;         add byte[bp - 3], 8
;         cmp byte[bp - 3], 255
;         inc word[bp - 1]
;         jne r_loop
; palette_loop_end:
;         add sp, 6
;         pop bp
;         ret

fname   db      "MONA    CFG"
vesares dw      800
vesabpp db      32

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
        mov  esp, 0x80000       ; sp is 3MB
        push eax
        jmp  KERNEL_ADDR
;  hang:
;          jmp hang

buffer  times KERNEL_ADDR-($-$$) db 0
