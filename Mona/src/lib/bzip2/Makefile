TARGET  = $(LIBDIR)/libbz2.a
SOURCES = blocksort.c  \
          huffman.c    \
          crctable.c   \
          randtable.c  \
          compress.c   \
          decompress.c \
          bzlib.c

MONADIR = ../../..
include $(MONADIR)/env/Makefile.inc
include $(MONADIR)/env/dirnames.inc

BIGFILES = -D_FILE_OFFSET_BITS=64
CFLAGS  += -Winline -ffreestanding -fomit-frame-pointer -fno-strength-reduce $(BIGFILES) -DBZ_NO_STDIO
OBJECTS  = $(SOURCES:.c=.o)
INCLUDE  = 

all : $(TARGET)

$(TARGET): $(OBJECTS)
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.SUFFIXES: .c .o
.c.o:
	$(CC) -c -o $@ $(CFLAGS) $(INCLUDE) $<

depend:
	$(CC) -MM $(INCLUDE) $(CFLAGS) $(SOURCES) > dependencies

install: $(TARGET)

include dependencies
