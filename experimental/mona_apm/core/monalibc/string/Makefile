CFLAGS = -nostdinc -I../include -fno-builtin -I$(MONADIR)/include
CC = i386-mingw32-gcc

include SOURCES.inc
OBJECTS = $(SOURCES:.c=.o)

all: $(OBJECTS)

.c.o:
	$(CC) -c -o $@ $<  $(CFLAGS)

clean:
	rm -rf $(OBJECTS)
