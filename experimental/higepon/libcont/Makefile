TARGET  = ./cont

SOURCES = \
main.c \
mysetjmp.c\
cont.c\
jmp_outside_test.c\
jmp_inside_test.c

CFLAGS = -g -idirafter . -Wall -O0

OBJECTS = $(SOURCES:.c=.o)

all : $(TARGET)

$(TARGET): $(OBJECTS) mysetjmp_asm.o
	$(CC) mysetjmp_asm.o $(OBJECTS) -o $(TARGET)

.SUFFIXES: .c .o
.cpp.o:
	$(CC) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

mysetjmp_asm.o: mysetjmp.asm
	nasm $< -o $@ -felf -g -F stabs

depend:
	$(CC) -MM $(INCLUDE) $(CFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(TARGET) mysetjmp_asm.o

-include dependencies
