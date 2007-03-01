GC_STACK_TEST01 = ./gc_stack_test_01
GC_STACK_TEST01_OBJECTS = $(GC_OBJECTS) $(GC_STACK_TEST01).o

CXXFLAGS = -pg -g -idirafter . -Wall $(GC_FLAGS) -DGC_TEST #-O3 # O2じゃないと gc 

GC_SOURCES = GCNode.cpp gc.cpp
GC_OBJECTS = $(GC_SOURCES:.cpp=.o)

all : $(GC_STACK_TEST01)

$(GC_STACK_TEST01): $(GC_STACK_TEST01_OBJECTS)
	$(CXX) -pg  $(GC_STACK_TEST01_OBJECTS) -o $@

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -g $(CXXFLAGS) $(INCLUDE) -c $< -o $@

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(GC_SOURCES) $(GC_STACK_TEST01).cpp > dependencies

clean :
	rm -f $(GC_OBJECTS) $(GC_STACK_TEST01).o $(GC_STACK_TEST01)

check : $(GC_STACK_TEST01)
	@$(GC_STACK_TEST01)

-include dependencies
