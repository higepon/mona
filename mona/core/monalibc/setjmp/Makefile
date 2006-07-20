SOURCES = setjmp.asm
OBJECTS = $(SOURCES:.asm=.o)

.SUFFIXES: .asm .o

all: $(OBJECTS)

clean:
	rm -rf $(OBJECTS)

.asm.o:
	nasm $< -o $@ -f coff
