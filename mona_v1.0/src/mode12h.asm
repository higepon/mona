bits 32

%include "macro.asm"

cglobal rectangle
cglobal put_pixel
cglobal scroll_up
cglobal write_font
cglobal pos_x
cglobal pos_y

; rectangle(int,x, int y,int xx, int yy, char color)
rectangle:
        push    ebp
        mov     ebp,esp
        pushad
        mov     ax,word [ebp+0x08]
        mov     bx,word [ebp+0x0c]
        mov     cl,byte [ebp+0x18]
        sub     esp,0x20
.rect0: mov     word [esp+0x00],ax
        mov     word [esp+0x04],bx
        mov     byte [esp+0x08],cl
        call    put_pixel
        inc     ax
        cmp     ax,word [ebp+0x10]
        jb      .rect0
        mov     ax,word [ebp+0x08]
        inc     bx
        cmp     bx,word [ebp+0x14]
        jb      .rect0
        add     esp,0x20
        popad
        mov     esp,ebp
        pop     ebp
        ret

; put_pixel(int x, int y, char color)
put_pixel:
        push    ebp
        mov     ebp,esp
        pushad
        mov     edi,0x000a0000
        xor     eax,eax
        mov     ecx,eax
        mov     edx,eax
        mov     ax,word [ebp+0x0c]
        mov     cx,0x0050
        mul     cx
        add     edi,eax
        mov     ax,word [ebp+0x08]
        mov     cl,0x08
        div     cl
        mov     cl,al
        add     edi,ecx
        mov     cl,ah
        mov     dx,0x03c4
        mov     ax,0x0f02
        out     dx,ax
        mov     dx,0x03ce
        mov     al,0x05
        out     dx,al
        inc     edx
        rol     ax,8
        in      al,dx
        ror     ax,8
        dec     edx
        and     ah,0xfc
        or      ah,0x02
        out     dx,ax
        mov     ax,0x8008
        shr     ah,cl
        out     dx,ax
        mov     al,byte [edi]
        mov     al,byte [ebp+0x10]
        mov     byte [edi],al
        popad
        mov     esp,ebp
        pop     ebp
        ret

; scroll_up() --- scroll up only for 16 dot font.
scroll_up:
        pushad
        mov     edi,0x000a0000
        mov     esi,0x000a0500
        mov     dx,0x03ce
        mov     al,0x05
        out     dx,al
        rol     ax,4
        inc     edx
        in      al,dx
        dec     edx
        ror     ax,4
        and     ah,0xfd
        or      ah,0x01
        out     dx,ax
        mov     dx,0x03da
.wait0: in      al,dx
        test    al,0x08
        jnz     .wait0
.wait1: in      al,dx
        test    al,0x08
        jz      .wait1
.scro0: mov     al,byte [esi]
        inc     esi
        mov     byte [edi],al
        inc     edi
        cmp     edi,0x000a9600
        jne     .scro0
        popad
        ret

; write_font(char character, char fontcolor, char backcolor)
write_font:
        push    ebp
        mov     ebp,esp
        pushad
        xor     eax,eax
        mov     edx,eax
        mov     edi,0x000a0000
        mov     al,byte [pos_y]
        mov     cx,0x0500
        mul     cx
        add     edi,eax
        mov     dl,byte [pos_x]
        add     edi,edx
        mov     dx,0x03c4
        mov     ax,0x0f02
        out     dx,ax
        mov     dx,0x03ce
        mov     al,0x05
        out     dx,al
        rol     ax,8
        inc     edx
        in      al,dx
        dec     edx
        ror     ax,8
        and     ah,0xfc
        or      ah,0x02
        out     dx,ax
        xor     eax,eax
        mov     al,byte [ebp+0x08]
        mov     esi,font+0x11
        shl     eax,4
        add     esi,eax
        mov     cl,0x10
.font0: mov     ax,0xff08
        out     dx,ax
        mov     bl,byte [edi]
        mov     bl,byte [ebp+0x10]
        mov     byte [edi],bl
        mov     ah,byte [esi]
        out     dx,ax
        mov     bl,byte [edi]
        mov     bl,byte [ebp+0x0c]
        mov     byte [edi],bl
        add     edi,0x50
        inc     esi
        dec     cl
        jnz     .font0
        popad
        mov     esp,ebp
        pop     ebp
        ret

pos_x  db      0
pos_y  db      0

font    incbin  "font/Gothn16x.tlf"
