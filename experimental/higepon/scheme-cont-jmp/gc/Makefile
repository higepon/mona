TARGET  = ./gc_test

SOURCES = \
main.cpp \
GCNode.cpp \
gc.cpp

CXXFLAGS = -pg -g -idirafter . -Wall $(GC_FLAGS) -O2 # O2じゃないと gc 

OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET)

$(TARGET): $(OBJECTS) Makefile
	$(CXX) -pg  $(OBJECTS) -o $(TARGET)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -g $(CXXFLAGS) $(INCLUDE) -c $< -o $@

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(TARGET)

-include dependencies
