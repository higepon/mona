NASM       = nasm
NDISASM    = ndisasm
CC         = /usr/bin/g++
LD         = /usr/bin/ld
STRIP      = /usr/bin/strip
CONTRIB_LIB= ./contrib/lib_cygwin
LFLAGS     = -n -Ttext 0x1200 -static
# for gcc3.x@cygwin
CFLAGS     = -nostdlib -fno-exceptions  -fno-rtti -Wall
ENV             = env
VERSION         = 0.1.1
DOXYGEN         = doxygen
BIN             = ../bin
LIST            = ../list
TOOLS           = ../tools
INCLUDE         = include
USERINCLUDE     = user/include
CNTRINCLUDE     = contrib/include
CFINCLUDE       = -I$(INCLUDE) -I$(USERINCLUDE) -I$(CNTRINCLUDE)
TARGET          = $(BIN)/mona.img
KERNEL_IMG      = $(BIN)/KERNEL.IMG
BOOT1           = $(BIN)/firstboot.bin
BOOT2           = $(BIN)/secondboot.bin
KERNEL          = $(BIN)/third.bin
HELLO           = $(BIN)/HELLO.ELF
KEYBOARD_SERVER = $(BIN)/KEYBDMNG.SVR
SHELL_SERVER    = $(BIN)/SHELL.SVR
JPEG_DEMO       = $(BIN)/JPEGDEMO.ELF
MAP_SERVER      = $(BIN)/MAP.SVR
MOUSE_SERVER    = $(BIN)/MOUSE.SVR
FAT_TOOL        = $(BIN)/fat_write.exe
TEMPLATE        = $(TOOLS)/fat_template.img
SOURCES      = cstart.cpp            \
               kernel.cpp            \
               ihandlers.cpp         \
               operator.cpp          \
               purevirtual.cpp       \
               checker.cpp           \
               string.cpp            \
               Semaphore.cpp         \
               FDCDriver.cpp         \
               GraphicalConsole.cpp  \
               pic.cpp               \
               io.cpp                \
               syscalls.cpp          \
               BitMap.cpp            \
               FAT12.cpp             \
               Process.cpp           \
               info.cpp              \
               GDTUtil.cpp           \
               IDTUtil.cpp           \
               PageManager.cpp       \
               elf.cpp               \
               z.cpp                 \
               MemoryManager.cpp     \
               sysresource.cpp       \
               Segments.cpp          \
               test_higepon.cpp      \
               Mutex.cpp             \
               VesaConsole.cpp       \

OBJECTS = $(SOURCES:.cpp=.o)

all : copy_userapp

copy_userapp : $(TARGET) $(HELLO) $(SHELL_SERVER) $(KEYBOARD_SERVER) $(JPEG_DEMO) $(MAP_SERVER) $(MOUSE_SERVER)
	(cd $(BIN); $(FAT_TOOL) mona.img HELLO.ELF)
	(cd $(BIN); $(FAT_TOOL) mona.img SHELL.SVR)
	(cd $(BIN); $(FAT_TOOL) mona.img KEYBDMNG.SVR)
	(cd $(BIN); $(FAT_TOOL) mona.img JPEGDEMO.ELF)
	(cd $(BIN); $(FAT_TOOL) mona.img MAP.SVR)
	(cd $(BIN); $(FAT_TOOL) mona.img MOUSE.SVR)

$(TARGET) : $(BOOT1) $(BOOT2) $(KERNEL) $(FAT_TOOL) userapp
	cat $(BOOT2) $(KERNEL) > $(KERNEL_IMG)
	(cat $(BOOT1);dd if=$(TEMPLATE) bs=512 skip=1 ) > $(TARGET)
	(cd $(BIN); $(FAT_TOOL) mona.img KERNEL.IMG)

$(FAT_TOOL) : $(TOOLS)/fat_write.cpp FAT12.cpp include/FAT12.h string.cpp BitMap.cpp
	$(CC) -DFS_TEST -o $(FAT_TOOL) $(TOOLS)/fat_write.cpp $(TOOLS)/HogeDriver.cpp FAT12.cpp string.cpp BitMap.cpp -idirafter include -idirafter $(USERINCLUDE) -idirafter $(TOOLS)

$(KERNEL) : $(OBJECTS) $(DRVROBJ) mode12h.o ihandler.o core.o
	$(LD) --Map kernel.map $(LFLAGS) -o$@ $(OBJECTS) mode12h.o ihandler.o core.o -L$(CONTRIB_LIB) -lz
#	cp $@ $(BIN)/mona.dbg
#	$(NDISASM) -u $@ > $(LIST)/third.list
	$(STRIP)  -O binary $@

userapp :
	(cd user; make)

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
	gcc -MM $(CFINCLUDE) $(CFLAGS) $(SOURCES) > $(ENV)/dependencies
	(cd user; make depend)

clean :
	rm -f $(OBJECTS) $(TARGET) $(BOOT1) $(BOOT2) $(KERNEL) $(KERNEL_IMG) $(FAT_TOOL) $(BIN)/mona.dbg mode12h.o ihandler.o core.o ../list/*.list
	(cd user; make clean)

dist :
	cd ../bin; zip  -9 ../../mona-$(VERSION)-image.zip  mona.img
	cd ../../; tar cf - mona_v1.0 | gzip -9 > "mona-$(VERSION).tar.gz"

write : copy_userapp
	cp $(TARGET) /dev/fd0

include $(ENV)/dependencies
