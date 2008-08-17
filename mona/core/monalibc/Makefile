SRCTOP=../..
include $(SRCTOP)/Makefile.inc

CRT_OBJECT = crt/crt.o
LIB_STATIC = libmonalibc.a
TARGET = $(LIB_STATIC)

ifneq ($(BUILD_TARGET),ELF)
LIB_IMPORT	= libmonalibc-imp.a
LIB_DYNAMIC	= MONALIBC.DLL
LIB_DYNAMIC5	= MONALIBC.DL5
TARGET		+= $(LIB_IMPORT) $(LIB_DYNAMIC) $(LIB_DYNAMIC5) $(CRT_OBJECT)
endif

include src.inc

OBJECTS = $(SOURCES:.c=.o) setjmp/setjmp.o crt/dllmain.o stdio/stream.o stdio/boshi/filelist.o
INCLUDE = -I. -I$(INCDIR) -I$(INCDIR)/monalibc -DMONA

LINK = -L$(SRCTOP)/lib -lmonapi-imp

all: $(TARGET)

$(LIB_STATIC): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

ifneq ($(BUILD_TARGET),ELF)
$(LIB_IMPORT): $(LIB_DYNAMIC)
$(LIB_DYNAMIC): $(OBJECTS)
	$(LD) -e _dllmain --export-all-symbols --enable-auto-import --out-implib $(LIB_IMPORT) -o $@ $(OBJECTS) $(LINK) -Map monalibc.map
	$(STRIP) $@

$(LIB_DYNAMIC5): $(LIB_DYNAMIC)
	bim2bin in:$(LIB_DYNAMIC) out:$@ -osacmp -tek5 BS:0 eprm:z0
endif

.SUFFIXES: .o .c
.c.o:
	$(CC) -c -o $@ $<  $(INCLUDE) -fno-builtin

crt: $(CRT_OBJECT)

crt_force:
	$(CC) -c -o crt/crt.o crt/crt.cpp $(INCLUDE)

$(CRT_OBJECT):
	$(CC) -c -o $@ $(CRT_OBJECT:.o=.cpp)  $(INCLUDE)

setjmp/setjmp.o: setjmp/setjmp.asm
	$(NASM) $< -o $@ -f coff

crt/dllmain.o: crt/dllmain.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) -fno-builtin

stdio/stream.o: stdio/stream.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) -fno-builtin
stdio/boshi/filelist.o: stdio/boshi/filelist.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS) -fno-builtin

depend dependencies:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean:
	rm -rf $(TARGET) $(OBJECTS) dependencies

install: $(TARGET)
	mkdir -p $(LIBDIR)
	$(INSTALL) -p -m 0644 $(LIB_STATIC) $(LIBDIR)/
ifneq ($(BUILD_TARGET),ELF)
	$(INSTALL) -p -m 0644 $(LIB_IMPORT) $(LIBDIR)/
	mkdir -p $(BINDIR)/LIBS
	$(INSTALL) -p -m 0644 $(LIB_DYNAMIC5) $(BINDIR)/LIBS/
endif
	$(INSTALL) -p $(CRT_OBJECT) $(LIBDIR)
	$(INSTALL) -p -m 0644 stdio/boshi/file.h $(INCDIR)/monalibc/

-include dependencies
