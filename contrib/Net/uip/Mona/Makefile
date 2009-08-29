ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../../../mona; pwd)
endif

TARGET  = UIP
SOURCES = main.cpp
INSTDIR = $(BINDIR)/APPS

SHAREDIR   = $(MONADIR)/share
IMPSFX = -imp
include $(SHAREDIR)/configs/monapi.inc

UIP_PATH = ../uip-0.9/uip
SERVER_PATH  =  ../../../NetServer/net/
INCLUDE += -idirafter . -idirafter $(WINPCAP_PATH)/Include -idirafter $(UIP_PATH) -idirafter $(SERVER_PATH) -idirafter $(MONADIR)/include/stlport -idirafter $(MONADIR)/include/monapi -idirafter $(MONADIR)/include

OBJECTS +=uip.o uip_arp.o uip_arch.o  httpd.o fs.o cgi.o uip_loop.o

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

ADDLINK += -lpci -lmonalibc

CXXFLAGS += -DDEBUG

# uip_loop is stub. An user of uIP who touches uip_xxx structs should be compiled with -fpack-struct option.
uip_loop.o: uip_loop.c
	$(CC) -o $@ $(CFLAGS) $(INCLUDE) -nostdinc -fpack-struct -c $<

uip.o: $(UIP_PATH)/uip.c $(UIP_PATH)/uip.h uipopt.h #$(SERVER_PATH)/ServerMessageLoop.h
	$(CC) -o uip.o $(CFLAGS) $(INCLUDE)  -nostdinc -fpack-struct -c $(UIP_PATH)/uip.c

httpd.o: $(UIP_PATH)/uip.c $(UIP_PATH)/uip.h uipopt.h #$(SERVER_PATH)/ServerMessageLoop.h
	$(CC) -o httpd.o $(CFLAGS) $(INCLUDE)  -nostdinc -fpack-struct -c ../uip-0.9/apps/httpd/httpd.c

fs.o: $(UIP_PATH)/uip.c $(UIP_PATH)/uip.h uipopt.h #$(SERVER_PATH)/ServerMessageLoop.h
	$(CC) -o fs.o $(CFLAGS) $(INCLUDE)  -nostdinc -fpack-struct -c ../uip-0.9/apps/httpd/fs.c

cgi.o: $(UIP_PATH)/uip.c $(UIP_PATH)//uip.h uipopt.h # $(SERVER_PATH)/ServerMessageLoop.h
	$(CC) -o cgi.o $(CFLAGS) $(INCLUDE)  -nostdinc -fpack-struct -c ../uip-0.9/apps/httpd/cgi.c


uip_arp.o: $(UIP_PATH)/uip_arp.c $(UIP_PATH)/uip_arp.h $(UIP_PATH)/uip.h uipopt.h # $(SERVER_PATH)/ServerMessageLoop.h
	$(CC) -o uip_arp.o $(CFLAGS) $(INCLUDE) -nostdinc -fpack-struct -c $(UIP_PATH)/uip_arp.c

# ServerMessageLoop.o: $(SERVER_PATH)/ServerMessageLoop.cpp ../uip/uip.h uipopt.h # $(SERVER_PATH)/ServerMessageLoop.h
# 	$(CXX) -o ServerMessageLoop.o $(CXXFLAGS) $(INCLUDE)  -nostdinc -c $(SERVER_PATH)/ServerMessageLoop.cpp

TcpClient.o: $(SERVER_PATH)/TcpClient.cpp $(SERVER_PATH)/TcpClient.h
	$(CXX) -o TcpClient.o $(CXXFLAGS) $(INCLUDE)  -nostdinc -c $(SERVER_PATH)/TcpClient.cpp

-include dependencies

