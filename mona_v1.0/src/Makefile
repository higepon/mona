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
VERSION    = 0.0.5
DOXYGEN    = doxygen
BIN        = ../bin
LIST       = ../list
INCLUDE    = include
STLINCLUDE = stlport
CFINCLUDE  = -I$(INCLUDE) -I$(STLINCLUDE)
TARGET     = $(BIN)/mona.img
BOOT1      = $(BIN)/firstboot.bin
BOOT2      = $(BIN)/secondboot.bin
KERNEL     = $(BIN)/third.bin
SOURCES    = cstart.cpp            \
             monaKernel.cpp        \
             monaIdt.cpp           \
             monaIhandler.cpp      \
             IA32MemoryManager.cpp \
             operator.cpp          \
             monaPureVirtual.cpp   \
             KeyBoardManager.cpp   \
             tester.cpp            \
             ProcessManager.cpp    \
             checker.cpp           \
             string.cpp            \
             SystemInfo.cpp        \
             Semaphore.cpp         \
             MFDCDriver.cpp        \
             GraphicalConsole.cpp  \
             kthread.cpp           \
             pic.cpp               \
             io.cpp                \
             rtc.cpp               \
             disp.cpp              \
             syscalls.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET)

$(TARGET) : $(BOOT1) $(BOOT2) $(KERNEL)
	cat  $(BOOT1) $(BOOT2) $(KERNEL) >$@
	cat $@ /dev/zero|dd of=$(BIN)/mona.tmp bs=1k count=1440
	mv $(BIN)/mona.tmp $@

$(KERNEL) : $(OBJECTS) memcmp.o memmove.o mode12h.o ihandler.o core.o
	$(LD) $(LFLAGS) -o$@ $(OBJECTS) memcmp.o memmove.o mode12h.o ihandler.o core.o
	$(NDISASM) -u $@ > $(LIST)/third.list
	$(STRIP)  -O binary $@

memcmp.o : ./FreeBSD/memcmp.c
	$(CC) $(CFLAGS) $(CFINCLUDE) -c ./FreeBSD/memcmp.c -o memcmp.o

memmove.o : ./FreeBSD/memmove.c
	$(CC) $(CFLAGS) $(CFINCLUDE) -c ./FreeBSD/memmove.c -o memmove.o

.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) $(CFINCLUDE) -c $<

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
	gcc -MM $(CFINCLUDE) $(CFLAGS) -- $(SOURCES) > $(ENV)/dependencies

clean :
	rm -f $(OBJECTS) $(TARGET) $(BOOT1) $(BOOT2) $(KERNEL) memmove.o memcmp.o mode12h.o ihandler.o core.o

dist :
	cd ../bin; zip  -9 ../../mona-$(VERSION)-image.zip  mona.img
	cd ../../; tar cf - mona_v1.0 | gzip -9 > "mona-$(VERSION).tar.gz"

include $(ENV)/dependencies
