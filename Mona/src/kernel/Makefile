MONADIR         = ../..

include $(MONADIR)/env/dirnames.inc
include $(MONADIR)/env/Makefile.inc
include $(MONADIR)/env/kernel.inc

KERNELDIR       = $(MONADIR)/src/kernel
INCLUDE         = -I$(INCDIR) -I$(KERNELDIR)
TARGET          = mona.img
KERNEL_IMG      = KERNEL.IMG
BOOT1           = firstboot.bin
BOOT2           = secondboot.bin
KERNEL          = third.bin
FAT_TOOL        = $(BINDIR)/fat_write.exe
TEMPLATE        = $(BINDIR)/fat_template.img
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
               Process.cpp           \
               info.cpp              \
               GDTUtil.cpp           \
               IDTUtil.cpp           \
               PageManager.cpp       \
               elf.cpp               \
               MemoryManager.cpp     \
               sysresource.cpp       \
               Segments.cpp          \
               test_higepon.cpp      \
               Mutex.cpp             \
               VesaConsole.cpp       \
               LogConsole.cpp        \
               fat.cpp               \
               fs.cpp                \
               IDManager.cpp         \
               KObject.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET)

$(TARGET) : $(BOOT1) $(BOOT2) $(KERNEL)
	cat $(BOOT2) $(KERNEL) > $(KERNEL_IMG)
	(cat $(BOOT1);dd if=$(TEMPLATE) bs=512 skip=1 ) > $(TARGET)
	$(FAT_TOOL) $(TARGET) $(KERNEL_IMG) $(KERNEL_IMG)

$(KERNEL) : $(OBJECTS) $(DRVROBJ) mode12h.o ihandler.o core.o
	$(LD) --Map kernel.map $(LFLAGS) -o$@ $(OBJECTS) mode12h.o ihandler.o core.o
#	cp $@ $(BINDIR)/mona.dbg
#	$(NDISASM) -u $@ > $(LISTDIR)/third.list
	$(STRIP)  -O binary $@

install : all
	cp -u $(TARGET) $(BINDIR)/$(TARGET)

.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) $(CXXFLAGS) $(MONA_DEVELOPER) $(INCLUDE) -c $<

$(BOOT1) : firstboot.asm
	date;
	$(NASM) firstboot.asm -o $@ -l $(LISTDIR)/first.list

$(BOOT2) : secondboot.asm
	$(NASM) secondboot.asm -o $@ -l $(LISTDIR)/second.list

mode12h.o : mode12h.asm
	$(NASM) $(NFLAGS) mode12h.asm  -o $@ -l $(LISTDIR)/mode12h.list

ihandler.o : ihandler.asm
	$(NASM) $(NFLAGS) ihandler.asm  -o $@ -l $(LISTDIR)/ihandler.list

core.o : core.asm
	$(NASM) $(NFLAGS) core.asm  -o $@ -l $(LISTDIR)/core.list

doxygen :
	$(DOXYGEN)

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(TARGET) $(BOOT1) $(BOOT2) $(KERNEL) $(KERNEL_IMG) $(BINDIR)/mona.dbg mode12h.o ihandler.o core.o $(LISTDIR)/*.list *.map

include ./dependencies
