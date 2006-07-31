SOURCES = errno.c
CFLAGS = -nostdinc -I../include -fno-builtin
CC = i386-mingw32-gcc
OBJECTS = $(SOURCES:.c=.o)

all: $(OBJECTS)

.c.o:
	$(CC) -c -o $@ $<  $(CFLAGS)

clean:
	rm -rf $(OBJECTS)
