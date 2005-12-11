TARGET = kernel.bin
ALIAS  = KERNEL.BIN

include $(MONADIR)/share/configs/Makefile.inc
include $(MONADIR)/share/configs/dirnames.inc

KERNEL_BASE_ADDR = 0x1000
REL_KERNEL_ADDR  = 0x0200
KERNEL_START_ADDR= 0x1200 # REL_KERNEL_ADDR + KERNEL_BASE_ADDR
MONA_CFG_ADDR    = 0x90000
LFLAGS       = -n -Ttext $(KERNEL_START_ADDR) -static
CXXFLAGS    += -DOSTYPE="\"$(OSTYPE)\"" -DREL_KERNEL_ADDR=$(REL_KERNEL_ADDR) -DKERNEL_BASE_ADDR=$(KERNEL_BASE_ADDR)
CXXFLAGS    += -DMONA_CFG_SIZE=$(MONA_CFG_SIZE) -DMONA_CFG_ADDR=$(MONA_CFG_ADDR)
INCLUDE      = -I.
SOURCES      = cstart.cpp            \
               kernel.cpp            \
               ihandlers.cpp         \
               operator.cpp          \
               purevirtual.cpp       \
               checker.cpp           \
               string.cpp            \
               Semaphore.cpp         \
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
               Segments.cpp          \
               Mutex.cpp             \
               VesaConsole.cpp       \
               LogConsole.cpp        \
               IDManager.cpp         \
               KObject.cpp           \
               Loader.cpp            \
               Scheduler.cpp         \
               RTC.cpp               \
               Messenger.cpp         \
               MemoryAllocator.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS) mode12h.o ihandler.o core.o
	$(LD) -e $(SYMPREFIX)cstart --Map kernel.map $(LFLAGS) -o $@ $(OBJECTS) mode12h.o ihandler.o core.o
#	$(NDISASM) -u $@ > kernel.lst
	$(STRIP) -O binary $@

.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

mode12h.o: mode12h.asm
	$(NASM) $(NFLAGS) mode12h.asm  -o $@ -l mode12h.lst

ihandler.o: ihandler.asm
	$(NASM) $(NFLAGS) ihandler.asm  -o $@ -l ihandler.lst

core.o: core.asm
	$(NASM) $(NFLAGS) core.asm  -o $@ -l core.lst

doxygen:
	$(DOXYGEN)

install: $(TARGET)
	mkdir -p $(BINDIR)
	$(INSTALL) $(TARGET) $(BINDIR)/$(ALIAS)
	mkdir -p $(INCDIR)/sys
	$(INSTALL) -m 0644 sys/*.h $(INCDIR)/sys/
	$(INSTALL) -m 0644 macro.asm $(INCDIR)/

uninstall:
	rm -f $(BINDIR)/$(ALIAS)
	for file in sys/*.h; \
	do \
	  rm -f $(INCDIR)/$$file; \
	done
	rm -f $(INCDIR)/macro.asm

depend dependencies:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean:
	rm -f $(TARGET) *.o *.lst *.map

-include dependencies
