TARGET  = tremor-test

SOURCES =    \
main.c       \
bitwise.c    \
block.c      \
codebook.c   \
floor0.c     \
floor1.c     \
framing.c    \
info.c       \
mapping0.c   \
mdct.c       \
registry.c   \
res012.c     \
sharedbook.c \
synthesis.c  \
vorbisfile.c \
window.c


CFLAGS = -g -idirafter . -Wall -D_LOW_ACCURACY_

OBJECTS = $(SOURCES:.c=.o)

all : $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

depend:
	$(CC) -MM $(INCLUDE) $(CFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(TARGET)

-include dependencies
