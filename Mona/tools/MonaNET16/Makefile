include MakeInclude

TARGET = I8086.dll secondboot.bin
ILOPT  = /OPT:1
ASMFLG = -DREL_KERNEL_ADDR=0x0800 -DMONA_HEADER_SIZE=0x10

all: .subdirs $(TARGET)

.subdirs: I8086.dll
	cd PEAnalyzerLib && $(MAKE)
	cd IL2Asm16 && $(MAKE)
	cd SecondBoot && $(MAKE)

secondboot.bin: secondboot.asm
	$(NASM) -o $@ $(ASMFLG) secondboot.asm

secondboot_.asm: secondboot.asm
	grep -v "^;" $< > $@

secondboot.asm: IL2Asm16.exe SecondBoot.exe secondboot32.asm
	$(NETRT) ./IL2Asm16.exe /MAIN:SecondBootMain $(ILOPT) /OUT:secondboot.as_ SecondBoot.exe
	mv secondboot.as_ $@
	cat secondboot32.asm >> $@

I8086.dll: I8086.cs
	$(CSC) /t:library $<

clean:
	cd PEAnalyzerLib && $(MAKE) $@
	cd IL2Asm16 && $(MAKE) $@
	cd SecondBoot && $(MAKE) $@
	rm -f $(TARGET) secondboot.as_ secondboot.asm

distclean: clean
	rm -f secondboot_.asm

install : all
	cp secondboot.asm ../../src/kernel/secondboot.asm
