TARGET  = $(MONADIR)/bin/getFileSize$(EXEEXT)
SOURCES = main.cpp

MONADIR = ../..
include $(MONADIR)/env/Makefile.inc
CXXFLAGS = -Wall -g
INCLUDE  = -idirafter .

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
