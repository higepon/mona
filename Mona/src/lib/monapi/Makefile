TARGET1 = $(LIBDIR)/monapi.o
TARGET2 = $(LIBDIR)/libmonapi.a
SOURCES = string.cpp \
          syscall.cpp \
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
          Keys.cpp

MONADIR  = ../../..
include $(MONADIR)/env/dirnames.inc
include $(MONADIR)/env/Makefile.inc

INCLUDE = -I$(INCDIR)
OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET1) $(TARGET2)

$(TARGET1):
	$(CXX) -o $@ -c $(CXXFLAGS) $(INCLUDE) monapi.cpp

$(TARGET2) : $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $<

clean :
	rm -f $(TARGET1) $(TARGET2) $(OBJECTS)

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) monapi.cpp $(SOURCES) > dependencies

install: $(TARGET1) $(TARGET2)

include dependencies
