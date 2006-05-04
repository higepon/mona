TARGET = secondboot.bin
ALIAS  = LOADER.BIN
SRCS   = SecondBoot.cs Str.cs A20.cs FDC.cs VESA.cs Console.cs
IL2A16 = IL2Asm16.exe
PEALIB = PEAnalyzerLib.dll
I8086  = I8086.dll
ILOPT  = /OPT:1

include $(MONADIR)/share/configs/Makefile.inc
include $(MONADIR)/share/configs/dirnames.inc
include $(DOTNETDIR)/Makefile.inc

all: $(TARGET)

$(TARGET): secondboot.asm
	$(NASM) $< -o $@ -l secondboot.lst

secondboot_.asm: secondboot.asm
	grep -v "^;" $< > $@

secondboot.asm: SecondBoot.exe secondboot32.asm $(PEALIB) $(IL2A16)
	$(NETRT) $(IL2A16) /MAIN:SecondBootMain $(ILOPT) /POST:secondboot32.asm /OUT:secondboot.as_ SecondBoot.exe
	mv secondboot.as_ $@

SecondBoot.exe: $(SRCS) $(I8086)
	$(CSC) $(CSCFLAGS) /reference:$(I8086) /out:$@ $(SRCS)

$(PEALIB): $(DOTNETDIR)/$(PEALIB)
	cp $< $@

$(IL2A16): $(DOTNETDIR)/$(IL2A16)
	cp $< $@

$(I8086): $(DOTNETDIR)/$(I8086)
	cp $< $@

clean:
	rm -f $(TARGET) secondboot.asm secondboot_.asm SecondBoot.exe *.lst $(PEALIB) $(IL2A16) $(I8086)

distclean: clean

install: $(TARGET)
	mkdir -p $(BINDIR)
	$(INSTALL) $(TARGET) $(BINDIR)/$(ALIAS)

uninstall:
	rm -f $(BINDIR)/$(ALIAS)