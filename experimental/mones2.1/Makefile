#$Id$
TARGET  = IPSTACK
SOURCES = main.cpp Nic.cpp Dispatch.cpp IPStack.cpp \
 L4Base.cpp ICMPInfo.cpp UDPInfo.cpp TCPInfo.cpp \
 MonADEC.cpp MonAMDpcn.cpp LoopBack.cpp NE2000.cpp
INSTDIR = $(BINDIR)/APPS

SHAREDIR   = $(MONADIR)/share
IMPSFX = -imp
include $(SHAREDIR)/configs/monapi.inc

INCLUDE += -idirafter . -I./include 

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $@ $(CXXFLAGS) $(INCLUDE) $<

.SUFFIXES: .c .o
.c.o:
	$(CC) -c -o $@ $(CFLAGS) $(INCLUDE) $<

.SUFFIXES: .asm .o
.asm.o:
	$(NASM) $(NFLAGS) -o $@ $<

all: $(TARGET).EX5
	cd lib && make
	cd client && make
	cd wkiss && make 
$(TARGET).EX5: $(TARGET).EXE
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

$(TARGET).EXE: $(OBJECTS) $(LINKDEP)
	$(LD) --Map $(TARGET).map $(LFLAGS) -o $@ $(LIBDIR)/monapi.o $(OBJECTS) -L$(LIBDIR) $(LINK)
	$(STRIP) $@

install: all
	mkdir -p $(INSTDIR)
	$(INSTALL) $(TARGET).EX5 $(INSTFILES) $(INSTDIR)
	cd lib && make
	cd client && make install
	cd wkiss && make install
run: install
	cd $(MONADIR)/tool/mkimg && make && $(MONADIR)/tool/mkimg/run.bat
runl: install
	cd $(MONADIR)/tool/mkimg && make && $(MONADIR)/tool/mkimg/runl.bat
runq: install
	cd $(MONADIR)/tool/mkimg && make && cp mona.iso $(QEMUDIR)
	cd $(QEMUDIR) && $(QEMUDIR)/mona.bat
runv: install
	cd $(MONADIR)/tool/mkimg && make && $(MONADIR)/tool/mkimg/runv.bat
kernel:
	cd $(MONADIR) && make
clean:
	rm -f $(OBJECTS) $(TARGET).EXE $(TARGET).EX5 $(CLEANFILES) $(TARGET).map
	rm -f ${MONADIR}/bin/mona.img dependencies
	cd lib && make clean
	cd client && make clean
	cd wkiss && make clean

depend dependencies:
ifneq ($(SOURCES),)
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) >> dependencies
endif
ifneq ($(CSOURCES),)
	$(CC) -MM $(INCLUDE) $(CFLAGS) $(SOURCES) >> dependencies
endif
ifneq ($(NASMSRCS),)
	for asm in $(NASMSRCS); do echo `echo $$asm | sed "s/\.asm/.o/`": "$$asm >> dependencies; done
endif

ADDLINK +=  -lPci -lmonalibc

CXXFLAGS += -DDEBUG

-include dependencies

