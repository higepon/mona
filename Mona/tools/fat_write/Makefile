TARGET  = $(MONADIR)/bin/fat_write$(EXEEXT)
SOURCES = main.cpp VirtualFloppy.cpp fat.cpp

MONADIR = ../..
include $(MONADIR)/env/Makefile.inc
CXXFLAGS = -Wall -g
INCLUDE  = -idirafter . -idirafter ../../include/sys

OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)
#	strip $(TARGET)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(TARGET)

include dependencies
