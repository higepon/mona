TARGET  = MonaServer.exe
SOURCES = \
	BitMap.cpp \
	MonaServer.cpp \
	VesaConsole.cpp \
	info.cpp \
	kernel.cpp \
	string.cpp \
	syscalls.cpp

MONADIR = ../..
include $(MONADIR)/env/Makefile.inc
CXXFLAGS = -Wall -DOSTYPE="\"$(OSTYPE)\""
INCLUDE  = -I. -I$(MONADIR)/include -I$(MONADIR)/include/stlport

OBJECTS = $(SOURCES:.cpp=.o) resource.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -mno-cygwin -mwindows $(OBJECTS) -o $(TARGET)
	strip $(TARGET)

resource.o: resource.rc
	windres $< -o $@

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

#depend:
#	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(TARGET)

#include dependencies
