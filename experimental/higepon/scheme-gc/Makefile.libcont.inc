LIBCONT_SOURCES = libcont/cont.c libcont/mysetjmp.c
LIBCONT_OBJECTS = $(LIBCONT_SOURCES:.c=.o) libcont/mysetjmp_asm.o

libcont/mysetjmp_asm.o: libcont/mysetjmp.asm
	nasm $< -o $@ -felf -g -F stabs
