
%include	'syscall.inc'

	cglobal	__syscall_0
	cglobal	__syscall_1
	cglobal	__syscall_2
	cglobal	__syscall_3

section	.text align = 16

				align	16
__syscall_0:	push	ebx
				mov		ebx, [esp + 4 + 4]
				int		0x80
				pop		ebx
				ret

				align	16
__syscall_1:	push	ebx
				push	esi
				mov		ebx, [esp + 8 + 4]
				mov		esi, [esp + 8 + 8]
				int		0x80
				pop		esi
				pop		ebx
				ret

				align	16
__syscall_2:	push	ebx
				push	esi
				mov		ebx, [esp + 8 + 4]
				mov		esi, [esp + 8 + 8]
				mov		ecx, [esp + 8 + 12]
				int		0x80
				pop		esi
				pop		ebx
				ret

				align	16
__syscall_3:	push	ebx
				push	esi
				push	edi
				mov		ebx, [esp + 12 + 4]
				mov		esi, [esp + 12 + 8]
				mov		ecx, [esp + 12 + 12]
				mov		edi, [esp + 12 + 16]
				int		0x80
				pop		edi
				pop		esi
				pop		ebx
				ret

	ends

