SOURCES = string.cpp \
          monapi.cpp \
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
          VirtualScreen.cpp \
          mt19937ar.cpp

MONADIR  = ../../..
include $(MONADIR)/env/dirnames.inc
include $(MONADIR)/env/Makefile.inc

TARGET  = $(LIBDIR)/libmonapi.a

INCLUDE = -I$(INCDIR)
OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $<

clean :
	rm -f $(TARGET) $(OBJECTS)

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

include dependencies
