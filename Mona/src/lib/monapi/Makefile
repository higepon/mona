CRT_OBJECT  = $(LIBDIR)/monapi.o
LIB_STATIC  = $(LIBDIR)/libmonapi.a
TARGET      = $(CRT_OBJECT) $(LIB_STATIC)

ifeq ($(BUILD_TARGET),PE-DLL)
LIB_IMPORT  = $(LIBDIR)/libmonapi-imp.a
LIB_DYNAMIC = MONAPI.DLL
TARGET     += $(LIB_IMPORT) $(LIB_DYNAMIC)
endif

SOURCES = monapi_impl.cpp \
          string.cpp \
          syscall.cpp \
          CString.cpp \
          FileInputStream.cpp \
          FileOutputStream.cpp \
          Floppy.cpp \
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
          cmemoryinfo.cpp

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

ifeq ($(BUILD_TARGET),PE-DLL)
$(LIB_IMPORT): $(LIB_DYNAMIC)
$(LIB_DYNAMIC): $(OBJECTS)
	dllwrap -e dllmain --export-all --implib $(LIB_IMPORT) -o $@ $(OBJECTS)
	$(STRIP) $@
endif

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $<

clean:
	rm -f $(TARGET) $(OBJECTS)

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) monapi.cpp $(SOURCES) > dependencies

install: $(TARGET)
ifeq ($(BUILD_TARGET),PE-DLL)
	mkdir -p $(MONADIR)/bin/root/LIBS
	$(INSTALL) $(LIB_DYNAMIC) $(MONADIR)/bin/root/LIBS
endif

include dependencies
