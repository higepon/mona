include SOURCES.inc
include $(MONADIR)/share/configs/Makefile.inc

CFLAGS = -nostdinc -I../include -fno-builtin -I$(MONADIR)/include
CC = i386-mingw32-gcc

OBJECTS = $(SOURCES:.c=.o)

all: $(OBJECTS)

.c.o:
	$(CC) -c -o $@ $<  $(CFLAGS)

clean:
	rm -rf $(OBJECTS)
