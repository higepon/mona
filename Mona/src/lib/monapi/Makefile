CRT_OBJECT   = $(LIBDIR)/monapi.o
LIB_STATIC   = $(LIBDIR)/libmonapi.a
TARGET       = $(CRT_OBJECT) $(LIB_STATIC)

ifneq ($(BUILD_TARGET),ELF)
LIB_IMPORT   = $(LIBDIR)/libmonapi-imp.a
LIB_DYNAMIC  = MONAPI.DLL
LIB_DYNAMIC5 = MONAPI.DL5
TARGET      += $(LIB_IMPORT) $(LIB_IMPORT) $(LIB_DYNAMIC) $(LIB_DYNAMIC5)
endif

SOURCES = monapi_impl.cpp \
          string.cpp \
          syscall.cpp \
          CString.cpp \
          MemoryManager.cpp \
          MemoryMap.cpp \
          Message.cpp \
          MonaApplication.cpp \
          Mutex.cpp \
          Observable.cpp \
          Random.cpp \
          Screen.cpp \
          System.cpp \
          VirtualScreen.cpp \
          mt19937ar.cpp \
          Keys.cpp \
          messages.cpp \
          cmemoryinfo.cpp \
          io.cpp

MONADIR  = ../../..
include $(MONADIR)/env/dirnames.inc
include $(MONADIR)/env/Makefile.inc

INCLUDE = -I$(INCDIR)
OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET)

$(CRT_OBJECT): monapi.cpp
	$(CXX) -o $@ -c $(CXXFLAGS) $(INCLUDE) monapi.cpp

$(LIB_STATIC): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

ifneq ($(BUILD_TARGET),ELF)
$(LIB_IMPORT): $(LIB_DYNAMIC)
$(LIB_DYNAMIC): $(OBJECTS)
	$(LD) --export-all-symbols --out-implib $(LIB_IMPORT) -o $@ $(OBJECTS)
	$(STRIP) $@

$(LIB_DYNAMIC5): $(LIB_DYNAMIC)
#	bzip2 -c $(LIB_DYNAMIC) > $@
	$(MONADIR)/bin/bim2bin in:$(LIB_DYNAMIC) out:$@ -osacmp -tek5 BS:0 eprm:z0
endif

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $<

clean:
	rm -f $(TARGET) $(OBJECTS)

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) monapi.cpp $(SOURCES) > dependencies

install: $(TARGET)
ifneq ($(BUILD_TARGET),ELF)
	mkdir -p $(MONADIR)/bin/root/LIBS
	mkdir -p $(MONADIR)/bin/iso/LIBS
	$(INSTALL) $(LIB_DYNAMIC5) $(MONADIR)/bin/root/LIBS
	$(INSTALL) $(LIB_DYNAMIC5) $(MONADIR)/bin/iso/LIBS
endif

include dependencies
