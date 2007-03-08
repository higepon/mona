GC_STACK_TEST01 = ./gc_stack_test_01

GC_TESTS = $(GC_STACK_TEST01)
GC_STACK_TEST01_OBJECTS  = $(GC_OBJECTS) $(GC_STACK_TEST01).o

CXXFLAGS = -pg -g -idirafter . -Wall $(GC_FLAGS) -DGC_TEST #-DGC_TRACE #-O3 # O2じゃないと gc 
CFLAGS   = $(CXXFLAGS)

GC_SOURCES = gc.cpp
GC_OBJECTS = $(GC_SOURCES:.cpp=.o) $(MYSETJMP_OBJECTS)

MYSETJMP_SOURCES = mysetjmp.c
MYSETJMP_OBJECTS = $(MYSETJMP_SOURCES:.c=.o) mysetjmp_asm.o

GC_TEST_OBJECTS = $(GC_TESTS:=.o)
GC_TEST_SOURCES = $(GC_TESTS:=.cpp)

all : $(GC_TESTS)

$(GC_STACK_TEST01): $(GC_STACK_TEST01_OBJECTS)
	$(CXX) -pg  $(GC_STACK_TEST01_OBJECTS) -o $@

mysetjmp_asm.o: mysetjmp.asm
	nasm $< -o $@ -felf -g -F stabs

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -g $(CXXFLAGS) $(INCLUDE) -c $< -o $@

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(GC_SOURCES) $(GC_TEST_SOURCES) > dependencies

clean :
	rm -f $(GC_OBJECTS) $(GC_TEST_OBJECTS) $(GC_TESTS)

check : $(GC_TESTS)
#	@ruby -e '"$(GC_TESTS)".split.each{ |s| puts `#{s}`}'
	@gosh -e '(begin (for-each sys-system (string-split "$(GC_TESTS)" #\SPACE)) (exit))'

-include dependencies
