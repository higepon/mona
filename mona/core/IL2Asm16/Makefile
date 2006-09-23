SRCTOP=../..
include $(SRCTOP)/Makefile.inc

TARGET = IL2Asm16.exe I8086.dll
SRCS   = IL2Asm16.cs X86Code.cs X86Codes.cs Util.cs
PEALIB = PEAnalyzerLib.dll

all: $(TARGET)

IL2Asm16.exe: $(SRCS) $(PEALIB)
	$(CSC) /reference:$(PEALIB) /out:$@ $(SRCS)

I8086.dll: I8086.cs
	$(CSC) /target:library $<

$(PEALIB): $(DOTNETDIR)/$(PEALIB)
	cp $< $@

clean:
	rm -f $(TARGET) $(PEALIB)

distclean: clean

install: $(TARGET)
	$(INSTALL) -p $(TARGET) $(DOTNETDIR)/

uninstall:
	rm -f $(DOTNETDIR)/IL2Asm16.exe
	rm -f $(DOTNETDIR)/I8086.dll
