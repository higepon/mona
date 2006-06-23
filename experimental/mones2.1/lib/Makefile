#$Id: Makefile 3292 2006-06-20 16:39:59Z eds1275 $
TARGET  = libNetClient.a
SOURCES = NetClient.cpp

SHAREDIR   = $(MONADIR)/share
IMPSFX = -imp
include $(SHAREDIR)/configs/monapi.inc

INCLUDE += -idirafter . -I../include 

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $@ $(CXXFLAGS) $(INCLUDE) $<

.SUFFIXES: .c .o
.c.o:
	$(CC) -c -o $@ $(CFLAGS) $(INCLUDE) $<

.SUFFIXES: .asm .o
.asm.o:
	$(NASM) $(NFLAGS) -o $@ $<
	

$(TARGET): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@
clean:
	rm -f $(TARGET) $(OBJECTS) dependencies

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

