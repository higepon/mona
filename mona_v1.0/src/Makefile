ENV        = env
ifdef CYGWIN
    include $(ENV)/cygwin.env
else
    ifdef DJGPP
        include $(ENV)/djgpp.env
    else
        include $(ENV)/linux.env
    endif
endif
include env.local
VERSION    = 0.0.8
DOXYGEN    = doxygen
BIN        = ../bin
LIST       = ../list
TOOLS      = ../tools
INCLUDE    = include
STLINCLUDE = stlport
CFINCLUDE  = -I$(INCLUDE) -I$(STLINCLUDE)
TARGET     = $(BIN)/mona.img
KERNEL_IMG = $(BIN)/KERNEL.IMG
BOOT1      = $(BIN)/firstboot.bin
BOOT2      = $(BIN)/secondboot.bin
KERNEL     = $(BIN)/third.bin
FAT_TOOL   = $(BIN)/fat_write.exe
TEMPLATE   = $(TOOLS)/fat_template.img
SOURCES    = cstart.cpp            \
             kernel.cpp            \
             idt.cpp               \
             ihandlers.cpp         \
             IA32MemoryManager.cpp \
             operator.cpp          \
             purevirtual.cpp       \
             KeyBoardManager.cpp   \
             tester.cpp            \
             ProcessManager.cpp    \
             checker.cpp           \
             string.cpp            \
             SystemInfo.cpp        \
             Semaphore.cpp         \
             FDCDriver.cpp         \
             GraphicalConsole.cpp  \
             kthread.cpp           \
             pic.cpp               \
             io.cpp                \
             rtc.cpp               \
             disp.cpp              \
             syscalls.cpp          \
             BitMap.cpp            \
             FAT12.cpp             \
             IDEDriver.cpp         \
             test_higepon.cpp      \
             test_mjt.cpp          \
             sysresource.cpp       \
             ISADriver.cpp         \
             info.cpp              \
             GDTUtil.cpp

DRVRSRC =    driver/ISADevice/PS2KBC/ps2kbc.cpp driver/ISADevice/PCIC/pcic.cpp
DRVROBJ = ps2kbc.o pcic.o

OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET)

$(TARGET) : $(BOOT1) $(BOOT2) $(KERNEL) $(FAT_TOOL)
	cat $(BOOT2) $(KERNEL) > $(KERNEL_IMG)
#	cp  -f $(TEMPLATE) $(TARGET).tmp
	(cat $(BOOT1);dd if=$(TEMPLATE) bs=512 skip=1 ) > $(TARGET)
	$(FAT_TOOL) $(TARGET)
#	(cat $(BOOT1);dd if=$(TARGET).tmp bs=512 skip=1 ) > $(TARGET)
#	rm -f $(TARGET).tmp

$(FAT_TOOL) : $(TOOLS)/fat_write.cpp FAT12.cpp include/FAT12.h string.cpp BitMap.cpp
	$(CC) -DFS_TEST -o $(FAT_TOOL) $(TOOLS)/fat_write.cpp $(TOOLS)/HogeDriver.cpp FAT12.cpp string.cpp BitMap.cpp -idirafter include -idirafter $(TOOLS)

$(KERNEL) : $(OBJECTS) $(DRVROBJ) memcmp.o memmove.o mode12h.o ihandler.o core.o
	$(LD) --Map kernel.map $(LFLAGS) -o$@ $(OBJECTS) memcmp.o memmove.o mode12h.o ihandler.o core.o $(DRVROBJ)
	$(NDISASM) -u $@ > $(LIST)/third.list
	$(STRIP)  -O binary $@

ps2kbc.o : driver/ISADevice/PS2KBC/ps2kbc.cpp
	$(CC) $(CFLAGS) $(CFINCLUDE) -c driver/ISADevice/PS2KBC/ps2kbc.cpp -o ps2kbc.o


pcic.o : driver/ISADevice/PCIC/pcic.cpp
	$(CC) $(CFLAGS) $(CFINCLUDE) -c driver/ISADevice/PCIC/pcic.cpp -o pcic.o

memcmp.o : ./FreeBSD/memcmp.c
	$(CC) $(CFLAGS) $(CFINCLUDE) -c ./FreeBSD/memcmp.c -o memcmp.o

memmove.o : ./FreeBSD/memmove.c
	$(CC) $(CFLAGS) $(CFINCLUDE) -c ./FreeBSD/memmove.c -o memmove.o

.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) $(MONA_DEVELOPER) $(CFINCLUDE) -c $<

$(BOOT1) : firstboot.asm
	$(NASM) firstboot.asm -o $@ -l $(LIST)/first.list

$(BOOT2) : secondboot.asm
	$(NASM) secondboot.asm -o $@ -l $(LIST)/second.list

mode12h.o : mode12h.asm
	$(NASM) mode12h.asm -felf -o $@ -l $(LIST)/mode12h.list

ihandler.o : ihandler.asm
	$(NASM) ihandler.asm -felf -o $@ -l $(LIST)/ihandler.list

core.o : core.asm
	$(NASM) core.asm -felf -o $@ -l $(LIST)/core.list

doxygen :
	$(DOXYGEN)

depend:
	gcc -MM $(CFINCLUDE) $(CFLAGS) -- $(SOURCES) $(DRVRSRC) > $(ENV)/dependencies

clean :
	rm -f $(OBJECTS) $(TARGET) $(BOOT1) $(BOOT2) $(KERNEL) $(KERNEL_IMG) $(FAT_TOOL) memmove.o memcmp.o mode12h.o ihandler.o core.o $(DRVROBJ)

dist :
	cd ../bin; zip  -9 ../../mona-$(VERSION)-image.zip  mona.img
	cd ../../; tar cf - mona_v1.0 | gzip -9 > "mona-$(VERSION).tar.gz"

write : $(TARGET)
	cp $(TARGET) /dev/fd0

include $(ENV)/dependencies
