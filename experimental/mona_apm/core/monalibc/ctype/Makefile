SOURCES = \
isalnum.c \
isalpha.c \
isascii.c \
isblank.c \
iscntrl.c \
isdigit.c \
isgraph.c \
islower.c \
isprint.c \
ispunct.c \
isspace.c \
isupper.c \
isxdigit.c \
tolower.c \
toupper.c
CFLAGS = -nostdinc -I../include -fno-builtin
CC = i386-mingw32-gcc
OBJECTS = $(SOURCES:.c=.o)

all: $(OBJECTS)

.c.o:
	$(CC) -c -o $@ $<  $(CFLAGS)

clean:
	rm -rf $(OBJECTS)
