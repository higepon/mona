TARGET  = UIP
SOURCES = main.cpp NE2000.cpp NicFactory.cpp NicServer.cpp monadev.cpp
INSTDIR = $(BINDIR)/APPS

SHAREDIR   = $(MONADIR)/share
IMPSFX = -imp
include $(SHAREDIR)/configs/monapi.inc

UIP_PATH = ../uip
INCLUDE += -idirafter . -idirafter $(WINPCAP_PATH)/Include -idirafter $(UIP_PATH) -idirafter ../apps/httpd -idirafter ../apps/telnet

OBJECTS +=fs.o httpd.o uip.o uip_arp.o cgi.o uip_arch.o

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

ADDLINK += -lmonalibc -lPci

CXXFLAGS += -DDEBUG

cgi.o: ../apps/httpd/cgi.c ../uip/uip.h uipopt.h ../apps/smtp/smtp.h \
 ../apps/httpd/cgi.h ../apps/httpd/httpd.h ../apps/httpd/fs.h
	$(CC) -o cgi.o $(CFLAGS) $(INCLUDE) -fpack-struct -c ../apps/httpd/cgi.c

uip.o: ../uip/uip.c ../uip/uip.h uipopt.h ../apps/httpd/httpd.h 
	$(CC) -o uip.o $(CFLAGS) $(INCLUDE) -fpack-struct -c ../uip/uip.c

uip_arp.o: ../uip/uip_arp.c ../uip/uip_arp.h ../uip/uip.h uipopt.h \
 ../apps/httpd/httpd.h
	$(CC) -o uip_arp.o $(CFLAGS) $(INCLUDE) -fpack-struct -c ../uip/uip_arp.c

fs.o: ../apps/httpd/fs.c ../uip/uip.h uipopt.h ../apps/smtp/smtp.h \
 ../apps/httpd/httpd.h ../apps/httpd/fs.h ../apps/httpd/fsdata.h \
 ../apps/httpd/fsdata.c
	$(CC) -o fs.o $(CFLAGS) $(INCLUDE) -fpack-struct -c ../apps/httpd/fs.c

httpd.o: ../apps/httpd/httpd.c ../uip/uip.h uipopt.h \
 ../apps/smtp/smtp.h ../apps/httpd/httpd.h ../apps/httpd/fs.h \
 ../apps/httpd/fsdata.h ../apps/httpd/cgi.h
	$(CC) -o httpd.o $(CFLAGS) $(INCLUDE) -fpack-struct -c ../apps/httpd/httpd.c	


-include dependencies

