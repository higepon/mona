TARGET  = RelocELF$(EXEEXT)
SOURCES = main.cpp elfparser.cpp

MONADIR = ../..
include $(MONADIR)/env/Makefile.inc
CXXFLAGS = -Wall -g -D_DEBUG_ELF
INCLUDE  = -idirafter . -idirafter ../../include

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
