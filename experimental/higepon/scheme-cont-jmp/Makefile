TARGET  = ./scheme
TEST_TARGET  = ./scheme_test

SOURCES = \
Application.cpp\
Assignment.cpp\
Begin.cpp\
Charcter.cpp\
Cond.cpp\
Definition.cpp\
Environment.cpp\
Error.cpp\
Frame.cpp\
Lambda.cpp\
Let.cpp\
LetAsterisk.cpp\
Macro.cpp\
MacroFilter.cpp\
Number.cpp\
Continuation.cpp\
Object.cpp\
Pair.cpp\
Parser.cpp\
procedures/CharcterProcedures.cpp\
procedures/EqualProcedures.cpp\
procedures/False.cpp\
procedures/IOProcedures.cpp\
procedures/NumberProcedures.cpp\
procedures/PairProcedures.cpp\
procedures/Set.cpp\
procedures/StringProcedures.cpp\
procedures/SymbolProcedures.cpp\
procedures/True.cpp\
procedures/VectorProcedures.cpp\
procedures/MiscProcedures.cpp\
Procedure.cpp\
Quote.cpp\
QuoteFilter.cpp\
SExp.cpp\
Kernel.cpp\
SpecialIf.cpp\
String.cpp\
Tokenizer.cpp\
Translator.cpp\
Variable.cpp\
Vector.cpp\
load.cpp\
scheme.cpp

TEST_SOURCES = \
test_cpp/yaml.cpp\
test_cpp/MacroMatchTest.cpp\
test_cpp/MacroFilterTest.cpp\
test_cpp/QuoteFilterTest.cpp

LIBCONT_SOURCES = \
libcont/cont.c\
libcont/mysetjmp.c

CXXFLAGS = -g -idirafter . -Wall -O0
CFLAGS   = $(CXXFLAGS)
# -fno-exceptions -fno-rtti -I /usr/local/include/gc -DGC_OPERATOR_NEW_ARRAY# -DUSE_BOEHM_GC

OBJECTS = $(SOURCES:.cpp=.o)
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)
LIBCONT_OBJECTS = $(LIBCONT_SOURCES:.c=.o) libcont/mysetjmp_asm.o

all : $(TARGET) $(TEST_TARGET)

$(TARGET): main.o $(OBJECTS) $(LIBCONT_OBJECTS)
	echo $(LIBCONT_OBJECTS)
	$(CXX) main.o $(OBJECTS) $(LIBCONT_OBJECTS) -o $(TARGET)

$(TEST_TARGET): test_cpp/main.o $(OBJECTS) $(LIBCONT_OBJECTS) $(TEST_OBJECTS)
	$(CXX) -lcppunit -ldl test_cpp/main.o $(OBJECTS) $(LIBCONT_OBJECTS) $(TEST_OBJECTS) -o $(TEST_TARGET)

libcont/mysetjmp_asm.o: libcont/mysetjmp.asm
	nasm $< -o $@ -felf -g -F stabs

check : $(TEST_TARGET) $(TARGET)
	$(TEST_TARGET)
	$(TARGET) test/test.scm

gdb : $(TARGET)
	gdb -x ./gdb.txt $(TARGET)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

test_cpp/main.o: test_cpp/main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(TEST_SOURCES) >> dependencies

clean :
	rm -f $(OBJECTS) $(TARGET) main.o $(TEST_OBJECTS) $(LIBCONT_OBJECTS) $(TEST_TARGET)

-include dependencies
