TARGET = ../../bin/fat_write.exe

CXX         = g++
CXXFLAGS    = -Wall
SRCDIR      = ../../src
INCLUDE     = -idirafter . -idirafter $(SRCDIR)/share
SOURCES     = main.cpp VirtualFloppy.cpp fat.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(TARGET)

include dependencies
