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
             monaVga.cpp           \
             monaIdt.cpp           \
             monaIhandler.cpp      \
             monaIo.cpp            \
             IA32MemoryManager.cpp \
             monaOperator.cpp      \
             monaPureVirtual.cpp   \
             monaUtil.cpp          \
             KeyBoardManager.cpp   \
             FDCDriver.cpp         \
             monaTester.cpp        \
             ProcessManager.cpp    \
             monaChecker.cpp       \
             string.cpp            \
             SystemInfo.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET)

$(TARGET) : $(BOOT1) $(BOOT2) $(KERNEL)
	cat  $(BOOT1) $(BOOT2) $(KERNEL) >$@
	cat $@ /dev/zero|dd of=$(BIN)/mona.tmp bs=1k count=1440
	mv $(BIN)/mona.tmp $@

$(KERNEL) : $(OBJECTS) memcmp.o memmove.o
	$(LD) $(LFLAGS) -o$@ $(OBJECTS) memcmp.o memmove.o
	$(NDISASM) -u $@ > $(LIST)/third.list
	$(STRIP)  -O binary $@

memcmp.o : ./FreeBSD/memcmp.c
	$(CC) $(CFLAGS) $(CFINCLUDE) -c ./FreeBSD/memcmp.c -o memcmp.o

memmove.o : ./FreeBSD/memmove.c
	$(CC) $(CFLAGS) $(CFINCLUDE) -c ./FreeBSD/memmove.c -o memmove.o

.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) $(CFINCLUDE) -c $<

$(BOOT1) : firstboot.asm boot.mac
	$(NASM) firstboot.asm -o $@ -l $(LIST)/first.list

$(BOOT2) : secondboot.asm
	$(NASM) secondboot.asm -o $@ -l $(LIST)/second.list

doxygen :
	$(DOXYGEN)

depend:
	gcc -MM $(CFINCLUDE) $(CFLAGS) -- $(SOURCES) > $(ENV)/dependencies

clean :
	rm -f $(OBJECTS) $(TARGET) $(BOOT1) $(BOOT2) $(KERNEL) memmove.o memcmp.o

include $(ENV)/dependencies
