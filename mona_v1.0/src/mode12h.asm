bits 32

global _wait_vsync
global _scroll_down
global _write_font

global _pos_x
global _pos_y

; wait_vsync();
;  wait for virtical sync. no argument.
_wait_vsync:
	pushad
	mov	dx,0x03da
.wait0:	in	al,dx
	test	al,0x08
	jnz	.wait0
.wait1:	in	al,dx
	test	al,0x08
	jz	.wait1
	popad
	ret

; scroll_down()
;  scroll down for 16 dot font. no argument.

_scroll_down:
	pushad
	mov	edi,0x000a0000
	mov	esi,0x000a0500
	mov	dx,0x03ce
	mov	al,0x05
	out	dx,al
	rol	ax,4
	inc	edx
	in	al,dx
	dec	edx
	ror	ax,4
	and	ah,0xfd
	or	ah,0x01
	out	dx,ax
.scro0:	mov	al,byte [esi]
	inc	esi
	mov	byte [edi],al
	inc	edi
	cmp	edi,0x000a9600
	jne	.scro0
	popad
	ret

; write_font(char character, char fgcolor, char bgcolor)
;  write character with color. upgrade version.
_write_font:
	push	ebp
	mov	ebp,esp
	pushad
	;
	xor	eax,eax
	mov	edx,eax
	mov	edi,0x000a0000
	mov	al,byte [_pos_y]
	mov	cx,0x0500
	mul	cx
	add	edi,eax
	mov	dl,byte [_pos_x]
	add	edi,edx			; write position
	mov	dx,0x03c4
	mov	ax,0x0f02
	out	dx,ax			; unmask all map
	mov	dx,0x03ce
	mov	al,0x05
	out	dx,al
	rol	ax,8
	inc	edx
	in	al,dx
	dec	edx
	ror	ax,8
	and	ah,0xfc
	or	ah,0x02			; change to write mode 2
	out	dx,ax
	xor	eax,eax
	mov	al,byte [ebp+0x08]
	mov	esi,dword [font_m]
	shl	eax,4
	add	esi,eax			; target font data
	mov	cl,0x10
.font0:	mov	ax,0xff08
	out	dx,ax			; fill background
	mov	bl,byte [edi]
	mov	bl,byte [ebp+0x10]
	mov	byte [edi],bl
	mov	ah,byte [esi]
	out	dx,ax
	mov	bl,byte [edi]
	mov	bl,byte [ebp+0x0c]
	mov	byte [edi],bl
	add	edi,0x50
	inc	esi
	dec	cl
	jnz	.font0
	;
	popad
	mov	esp,ebp
	pop	ebp
	ret

_pos_x	db	0
_pos_y	db	0

font_m	dd	font+0x11
font	incbin	"font/paw16a.fnt"

