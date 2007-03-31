TARGET        = ./scheme
TEST_TARGET   = ./scheme_test
REGISTERS_INC = ./register.inc
PROCEDURES_H  = ./primitive_procedures.h

CXXFLAGS = -pg -g -idirafter . -Wall -W $(GC_CFLAGS) #-DMACRO_TRACE
CFLAGS   = $(CXXFLAGS) -j2
LDFLAGS  = $(GC_LDFLAGS)

#include Makefile.boehm_gc.inc
#include Makefile.mona_gc.inc

# たまに これで make checkする
include Makefile.none_gc.inc

SOURCES = $(PROCEDURE_SOURCES) \
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
NamedLet.cpp\
LetAsterisk.cpp\
Macro.cpp\
MacroFilter.cpp\
Number.cpp\
Continuation.cpp\
Object.cpp\
Pair.cpp\
OutputPort.cpp\
InputPort.cpp\
Values.cpp\
Parser.cpp\
Procedure.cpp\
RiteralConstant.cpp\
Nil.cpp\
QuoteFilter.cpp\
SExp.cpp\
Kernel.cpp\
SpecialIf.cpp\
SString.cpp\
Tokenizer.cpp\
Translator.cpp\
Variable.cpp\
Vector.cpp\
Scanner.cpp\
SToken.cpp\
StringReader.cpp\
ExtRepParser.cpp\
Eof.cpp\
load.cpp\
scheme.cpp

PROCEDURE_SOURCES =\
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
procedures/Undef.cpp\
procedures/VectorProcedures.cpp\
procedures/MiscProcedures.cpp


TEST_SOURCES = \
test_cpp/yaml.cpp\
test_cpp/MacroMatchTest.cpp\
test_cpp/MacroFilterTest.cpp\
test_cpp/QuoteFilterTest.cpp\
test_cpp/ScannerTest.cpp\
test_cpp/ExtRepParserTest.cpp

LIBCONT_SOURCES = libcont/cont.c libcont/mysetjmp.c

OBJECTS = $(SOURCES:.cpp=.o)
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)
LIBCONT_OBJECTS = $(LIBCONT_SOURCES:.c=.o) libcont/mysetjmp_asm.o
GC_OBJECTS = $(GC_SOURCES:.cpp=.o)
all : $(TARGET) $(TEST_TARGET)

$(TARGET):  $(REGISTERS_INC) $(PROCEDURES_H) main.o $(OBJECTS) $(LIBCONT_OBJECTS) $(GC_OBJECTS)
	$(CXX) -pg -g main.o $(OBJECTS) $(LIBCONT_OBJECTS) $(GC_OBJECTS) -o $(TARGET) $(LDFLAGS)

$(TEST_TARGET):  $(REGISTERS_INC) $(PROCEDURES_H) test_cpp/main.o $(OBJECTS) $(LIBCONT_OBJECTS) $(TEST_OBJECTS) $(GC_OBJECTS)
	$(CXX) -lcppunit -ldl -g test_cpp/main.o $(OBJECTS) $(LIBCONT_OBJECTS) $(GC_OBJECTS) $(TEST_OBJECTS) -o $(TEST_TARGET) $(LDFLAGS)

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
	$(CXX) -g $(CXXFLAGS) $(INCLUDE) -c $< -o $@

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(REGISTERS_INC): $(PROCEDURE_SOURCES) ./test/generate.scm
	gosh ./test/generate.scm --register $(PROCEDURE_SOURCES) > $@

$(PROCEDURES_H): $(PROCEDURE_SOURCES) ./test/generate.scm
	gosh ./test/generate.scm --header $(PROCEDURE_SOURCES) > $@

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(TEST_SOURCES) >> dependencies

clean :
	rm -f $(OBJECTS) $(TARGET) main.o $(TEST_OBJECTS) $(LIBCONT_OBJECTS) $(TEST_TARGET) $(GC_OBJECTS) $(REGISTERS_INC) $(PROCEDURES_H)

-include dependencies
