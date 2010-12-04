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

OBJECTS = $(SOURCES:.c=.o) setjmp/setjmp.o crt/dllmain.o stdio/stream.o stdio/fclose.o stdio/fopen.o stdio/fread.o stdio/futil.o stdio/setvbuf.o stdio/boshi/filelist.o stdio/boshi/lowio.o socket.o dirent/opendir.o dirent/closedir.o dirent/dirutil.o dirent/readdir.o dirent/rewinddir.o dirent/telldir.o dirent/seekdir.o
INCLUDE = -I. -I$(INCDIR) -I$(INCDIR)/monalibc -I ./newlib -DMONA -D BUILD_MONALIBC

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

stdio/fclose.o: stdio/fclose.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) -fno-builtin

stdio/fread.o: stdio/fread.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) -fno-builtin

stdio/futil.o: stdio/futil.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS) -fno-builtin

stdio/setvbuf.o: stdio/setvbuf.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) -fno-builtin

stdio/fopen.o: stdio/fopen.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) -fno-builtin

socket.o: socket.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) -fno-builtin

stdio/boshi/filelist.o: stdio/boshi/filelist.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS) -fno-builtin

stdio/boshi/lowio.o: stdio/boshi/lowio.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS) -fno-builtin

dirent/opendir.o: dirent/opendir.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS) -fno-builtin

dirent/closedir.o: dirent/closedir.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS) -fno-builtin

dirent/seekdir.o: dirent/seekdir.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS) -fno-builtin

dirent/telldir.o: dirent/telldir.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS) -fno-builtin


dirent/dirutil.o: dirent/dirutil.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS) -fno-builtin

dirent/readdir.o: dirent/readdir.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS) -fno-builtin

dirent/rewinddir.o: dirent/rewinddir.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS) -fno-builtin

depend dependencies:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean:
	rm -rf $(TARGET) $(OBJECTS) dependencies
	rm -rf $(LIBDIR)/crt.o

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
