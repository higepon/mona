SRCTOP=../..
include $(SRCTOP)/Makefile.inc

TARGET  = bim2bin$(EXEEXT)
SOURCES = bim2bin.c

ifneq ($(BIM2BIN_CC),)
CC=$(BIM2BIN_CC)
else
CC=gcc
endif

CFLAGS = -Wall $(CFLAG_DEFS)
STRIP = strip
RM = -rm
INCLUDE  = -idirafter .

all : $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $(SOURCES)
	$(STRIP) $(TARGET)

clean:
	$(RM) -f $(TARGET)

distclean: clean

install: $(TARGET)
	mkdir -p $(PREFIX)/bin
	$(INSTALL) $(TARGET) $(PREFIX)/bin/

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)
