TARGET = firstboot.bin

include $(MONADIR)/share/configs/Makefile.inc
include $(MONADIR)/share/configs/dirnames.inc

all : $(TARGET)

$(TARGET): firstboot.asm
	$(NASM) $< -o $@ -l firstboot.lst

clean:
	rm -f $(TARGET) *.lst

distclean: clean

install: $(TARGET)
	mkdir -p $(BINDIR)
	$(INSTALL) $(TARGET) $(BINDIR)/

uninstall:
	rm -f $(BINDIR)/$(TARGET)
