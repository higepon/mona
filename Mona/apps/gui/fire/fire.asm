BITS 32
%include "../../../src/kernel/macro.asm"
cglobal DrawFire
cextern typeDraw
cextern FireSeed
cextern FireScreen
cextern calc
cextern pImage

DrawFire:
	pushad

	mov  esi, dword[FireScreen]
	add  esi, 2300h
	sub  esi, 80
	mov  ecx, 80
	xor  edx, edx
	
NEWLINE:
	mov  eax, dword[FireSeed]            ; New number
	mov  edx,8405h
	mul  edx
	inc  eax
	mov  dword[FireSeed], eax            ; Store seed
	
	mov  [esi], dl
	inc  esi
	dec  ecx
	jnz  NEWLINE
	
	mov  ecx, 2300h
	sub  ecx, 80
	mov  esi, dword[FireScreen]
	add  esi, 80
	
FIRELOOP:
	xor  eax, eax
	cmp  dword[typeDraw], 0
	jnz  notype1
	mov  al, [esi]
	add  al, [esi + 2]
	adc  ah, 0
	add  al, [esi + 1]
	adc  ah, 0
	add  al, [esi + 81]
	adc  ah, 0
	
notype1:
	cmp  dword[typeDraw], 1
	jnz  notype2
	mov  al, [esi]
	add  al, [esi - 1]
	adc  ah, 0
	add  al, [esi - 1]
	adc  ah, 0
	add  al, [esi + 79]
	adc  ah,0
	
notype2:
	cmp  dword[typeDraw], 2
	jnz  notype3
	mov  al, [esi]
	add  al, [esi - 1]
	adc  ah,0
	add  al, [esi + 1]
	adc  ah, 0
	add  al, [esi + 81]
	adc  ah,0
	
notype3:
	shr  eax, 2
	jz   ZERO
	dec  eax
	
ZERO:
	mov  [esi - 80], al
	inc  esi
	dec  ecx
	jnz  FIRELOOP
		
	mov  al, byte[calc]
	inc  al
	mov  byte[calc], al
	cmp  al,2
	jz   pdraw
	
	jmp  nodrw
	
pdraw:
	mov  byte[calc], 0
	
	mov	esi, dword[FireScreen]
	mov	edi, dword[pImage]
	mov	ebx, 100
	mov	edx, 80
	
newc1:
	push	ebx
	push	edx
	
newc2:
	movzx	eax, byte [esi]
	mov	ebx,eax
	mov	ecx,eax
	shl  	ax,8
	shr	bx,1
	mov	al,bl
	add	ecx,eax
	shl	ax,8
	mov	ch,ah
	
	mov	[edi],cx
	mov	[edi+3],cx
	shr	ecx,16
	mov	[edi+2],cl
	mov	[edi+2+3],cl
	add	edi,3*2
	
	inc	esi
	dec	edx
	jnz	newc2
	
	pop	edx
	pop	ebx
	dec	ebx
	jnz	newc1

	popad
	mov eax, 1
	ret
	
nodrw:
	popad
	mov eax, 0
	ret
