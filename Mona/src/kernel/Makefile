MONADIR         = ../..

include $(MONADIR)/env/dirnames.inc
include $(MONADIR)/env/Makefile.inc

KERNEL_BASE_ADDR = 0x1000
REL_KERNEL_ADDR  = 0x0200
KERNEL_START_ADDR= 0x1200 # REL_KERNEL_ADDR + KERNEL_BASE_ADDR
MONA_CFG_ADDR    = 0x90000
LFLAGS       = -n -Ttext $(KERNEL_START_ADDR) -static
CXXFLAGS    += -DOSTYPE="\"$(OSTYPE)\"" -DREL_KERNEL_ADDR=$(REL_KERNEL_ADDR) -DKERNEL_BASE_ADDR=$(KERNEL_BASE_ADDR)
CXXFLAGS    += -DMONA_CFG_SIZE=$(MONA_CFG_SIZE) -DMONA_CFG_ADDR=$(MONA_CFG_ADDR)
TOOLSDIR     = $(MONADIR)/tools
INCLUDE      = -I$(INCDIR) -I$(TOOLSDIR)
BOOT1        = firstboot.bin
BOOT2        = secondboot.bin
KERNEL       = kernel.bin
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
               GDTUtil.cpp           \
               IDTUtil.cpp           \
               PageManager.cpp       \
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
               KObject.cpp           \
               Loader.cpp            \
               Scheduler.cpp         \

OBJECTS = $(SOURCES:.cpp=.o)

all : $(BOOT1) $(BOOT2) $(KERNEL)

$(KERNEL) : $(OBJECTS) $(DRVROBJ) mode12h.o ihandler.o core.o
	$(LD) -e $(SYMPREFIX)cstart --Map kernel.map $(LFLAGS) -o $@ $(OBJECTS) mode12h.o ihandler.o core.o
#	$(NDISASM) -u $@ > $(LISTDIR)/third.list
	$(STRIP)  -O binary $@

install : all
	$(INSTALL) $(BOOT1) $(BINDIR)
	$(INSTALL) $(BOOT2) $(BINDIR)/root/LOADER.BIN
	$(INSTALL) $(KERNEL) $(BINDIR)/root/KERNEL.BIN

.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) $(CXXFLAGS) $(MONA_DEVELOPER) $(INCLUDE) -c $<

$(BOOT1) : firstboot.asm
	date;
	$(NASM) firstboot.asm -o $@ -l $(LISTDIR)/first.list

$(BOOT2) : secondboot.asm
	$(NASM) -DREL_KERNEL_ADDR=$(REL_KERNEL_ADDR) -DMONA_CFG_ADDR=$(MONA_CFG_ADDR) secondboot.asm -o $@ -l $(LISTDIR)/second.list

mode12h.o : mode12h.asm
	$(NASM) $(NFLAGS) mode12h.asm  -o $@ -l $(LISTDIR)/mode12h.list

ihandler.o : ihandler.asm
	$(NASM) $(NFLAGS) ihandler.asm  -o $@ -l $(LISTDIR)/ihandler.list

core.o : core.asm
	$(NASM) $(NFLAGS) core.asm  -o $@ -l $(LISTDIR)/core.list

doxygen :
	$(DOXYGEN)

depend :
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(BOOT1) $(BOOT2) $(KERNEL) mode12h.o ihandler.o core.o $(LISTDIR)/*.list *.map

include ./dependencies
