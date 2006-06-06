#$Id: Makefile 3230 2006-05-30 13:48:50Z eds1275 $
TARGET  = client
SOURCES = NetClient.cpp
INSTDIR = $(BINDIR)/APPS

SHAREDIR   = $(MONADIR)/share
IMPSFX = -imp
include $(SHAREDIR)/configs/monapi.inc

INCLUDE += -idirafter . 

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $@ $(CXXFLAGS) $(INCLUDE) $<

all: $(TARGET).EX5

$(TARGET).EX5: $(TARGET).EXE
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

$(TARGET).EXE: $(OBJECTS) $(LINKDEP)
	$(LD) --Map $(TARGET).map $(LFLAGS) -o $@ $(LIBDIR)/monapi.o $(OBJECTS) -L$(LIBDIR) $(LINK)
	$(STRIP) $@

install: all
	mkdir -p $(INSTDIR)
	$(INSTALL) $(TARGET).EX5 $(INSTFILES) $(INSTDIR)
clean:
	rm -f $(OBJECTS) $(TARGET).EXE $(TARGET).EX5 $(CLEANFILES) $(TARGET).map
	rm -f ${MONADIR}/bin/mona.img dependencies

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

ADDLINK += -lmonalibc

CXXFLAGS += -DDEBUG

-include dependencies

