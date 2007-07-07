ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

TARGET      = NET
INSTDIR     = $(BINDIR)/SERVERS

SOURCES     = main.cpp monadev.cpp MessageLoop.cpp
UIP_SOURCES = $(UIP_PATH)/uip/uip.c $(UIP_PATH)/uip/uip_arp.c uip_arch.c $(UIP_PATH)/apps/webclient/webclient.c $(UIP_PATH)/apps/webclient/http-user-agent-string.c $(UIP_PATH)/apps/webclient/http-strings.c

SHAREDIR   = $(MONADIR)/share
IMPSFX = -imp
include $(SHAREDIR)/configs/monapi.inc

UIP_PATH = uip-0.9

SERVER_PATH  = ../apps/net
INCLUDE += -idirafter . -idirafter $(UIP_PATH)/uip -idirafter $(UIP_PATH)/apps/webclient -idirafter $(UIP_PATH)/apps/resolv  -idirafter $(INCDIR)/stlport -idirafter $(INCDIR)/monalibc

UIP_OBJECTS = $(UIP_SOURCES:.c=.o)
OBJECTS += $(UIP_OBJECTS)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $@ $(CXXFLAGS)  -nostdinc $(INCLUDE) $<

.SUFFIXES: .c .o
.c.o:
	$(CC) -c -o $@ $(CFLAGS)  -nostdinc $(INCLUDE) $<

.SUFFIXES: .asm .o
.asm.o:
	$(NASM) $(NFLAGS) -o $@ $<

all: $(TARGET).EX5

$(TARGET).EX5: $(TARGET).EXE
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

$(TARGET).EXE: $(OBJECTS) $(LINKDEP)
	$(LD) --Map $(TARGET).map $(LFLAGS) -o $@ $(LIBDIR)/monapi_crt.o $(OBJECTS) -L$(LIBDIR) $(LINK)
	$(STRIP) $@

install: all
	mkdir -p $(INSTDIR)
	$(INSTALL) -p $(TARGET).EX5 $(INSTFILES) $(INSTDIR)

clean:
	rm -f $(OBJECTS) $(TARGET).EXE $(TARGET).EX5 $(CLEANFILES) $(TARGET).map

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

ADDLINK += -lpci -lmonalibc

-include dependencies

