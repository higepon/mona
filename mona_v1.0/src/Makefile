NASM       = nasm
NDISASM    = ndisasm
CC         = /usr/bin/g++
LD         = /usr/bin/ld
STRIP      = /usr/bin/strip
CONTRIB_LIB= ./contrib/lib_cygwin
LFLAGS     = -n -Ttext 0x1200 -static
# for gcc3.x@cygwin
CFLAGS     = -nostdlib -fno-exceptions  -fno-rtti -ffreestanding -Wall -g
ENV         = env
VERSION     = 0.1.1
DOXYGEN     = doxygen
BIN         = ../bin
LIST        = ../list
TOOLS       = ../tools
INCLUDE     = include
USERINCLUDE = user
CNTRINCLUDE = contrib/include
CFINCLUDE   = -I$(INCLUDE) -I$(USERINCLUDE) -I$(CNTRINCLUDE)
TARGET      = $(BIN)/mona.img
KERNEL_IMG  = $(BIN)/KERNEL.IMG
BOOT1       = $(BIN)/firstboot.bin
BOOT2       = $(BIN)/secondboot.bin
KERNEL      = $(BIN)/third.bin
USER_PROCESS= $(BIN)/HELLO.ELF
FAT_TOOL    = $(BIN)/fat_write.exe
TEMPLATE    = $(TOOLS)/fat_template.img
SOURCES     = cstart.cpp            \
              kernel.cpp            \
              ihandlers.cpp         \
              IA32MemoryManager.cpp \
              operator.cpp          \
              purevirtual.cpp       \
              KeyBoardManager.cpp   \
              tester.cpp            \
              checker.cpp           \
              string.cpp            \
              SystemInfo.cpp        \
              Semaphore.cpp         \
              FDCDriver.cpp         \
              GraphicalConsole.cpp  \
              pic.cpp               \
              io.cpp                \
              rtc.cpp               \
              disp.cpp              \
              syscalls.cpp          \
              BitMap.cpp            \
              FAT12.cpp             \
              test_higepon.cpp      \
              sysresource.cpp       \
              info.cpp              \
              GDTUtil.cpp           \
              IDTUtil.cpp           \
              Process.cpp           \
              Scheduler.cpp         \
              ProcessManager.cpp    \
              MessageServer.cpp     \
              PageManager.cpp       \
              elf.cpp               \
              test_mjt.cpp          \
              z.cpp                 \
              MemoryManager.cpp     \
              Segments.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET)

$(TARGET) : $(BOOT1) $(BOOT2) $(KERNEL) $(FAT_TOOL) $(USER_PROCESS)
	cat $(BOOT2) $(KERNEL) > $(KERNEL_IMG)
	(cat $(BOOT1);dd if=$(TEMPLATE) bs=512 skip=1 ) > $(TARGET)
	$(FAT_TOOL) $(TARGET)

$(FAT_TOOL) : $(TOOLS)/fat_write.cpp FAT12.cpp include/FAT12.h string.cpp BitMap.cpp
	$(CC) -DFS_TEST -o $(FAT_TOOL) $(TOOLS)/fat_write.cpp $(TOOLS)/HogeDriver.cpp FAT12.cpp string.cpp BitMap.cpp -idirafter include -idirafter $(USERINCLUDE) -idirafter $(TOOLS)

$(KERNEL) : $(OBJECTS) $(DRVROBJ) mode12h.o ihandler.o core.o userlib.o
	$(LD) --Map kernel.map $(LFLAGS) -o$@ $(OBJECTS) mode12h.o ihandler.o core.o userlib.o -L$(CONTRIB_LIB) -lz
	cp $@ $(BIN)/mona.dbg
	$(NDISASM) -u $@ > $(LIST)/third.list
	$(STRIP)  -O binary $@

userlib.o : ./user/userlib.cpp
	$(CC) $(CFLAGS) $(CFINCLUDE) -c ./user/userlib.cpp -o userlib.o

$(USER_PROCESS) : userlib.o ./user/hello.cpp
	$(CC) $(CFLAGS) $(CFINCLUDE) -c ./user/hello.cpp -o hello.o
	ld -n -Ttext 0xC0000000 -static hello.o userlib.o -o $(USER_PROCESS)
	objcopy --output-target=elf32-i386 $(USER_PROCESS)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) $(MONA_DEVELOPER) $(CFINCLUDE) -c $<

$(BOOT1) : firstboot.asm
	$(NASM) firstboot.asm -o $@ -l $(LIST)/first.list

$(BOOT2) : secondboot.asm
	$(NASM) secondboot.asm -o $@ -l $(LIST)/second.list

mode12h.o : mode12h.asm
	$(NASM) $(NFLAGS) mode12h.asm -felf -o $@ -l $(LIST)/mode12h.list

ihandler.o : ihandler.asm
	$(NASM) $(NFLAGS) ihandler.asm -felf -o $@ -l $(LIST)/ihandler.list

core.o : core.asm
	$(NASM) $(NFLAGS) core.asm -felf -o $@ -l $(LIST)/core.list

doxygen :
	$(DOXYGEN)

depend:
	gcc -MM $(CFINCLUDE) $(CFLAGS) -- $(SOURCES) > $(ENV)/dependencies

clean :
	rm -f $(OBJECTS) $(TARGET) $(BOOT1) $(BOOT2) $(KERNEL) $(KERNEL_IMG) $(FAT_TOOL) $(USER_PROCESS) $(BIN)/mona.dbg mode12h.o ihandler.o core.o hello.o userlib.o ../list/*.list

dist :
	cd ../bin; zip  -9 ../../mona-$(VERSION)-image.zip  mona.img
	cd ../../; tar cf - mona_v1.0 | gzip -9 > "mona-$(VERSION).tar.gz"

write : $(TARGET)
	cp $(TARGET) /dev/fd0

include $(ENV)/dependencies
