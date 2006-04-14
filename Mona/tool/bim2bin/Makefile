TARGET  = bim2bin$(EXEEXT)
SOURCES = bim2bin.c

include $(MONADIR)/share/configs/Makefile.inc

CFLAGS = -Wall $(CFLAG_DEFS)
CC = gcc
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
