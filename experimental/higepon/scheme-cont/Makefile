TARGET  = ./scheme
SOURCES = \
Application.cpp\
Assignment.cpp\
Begin.cpp\
Charcter.cpp\
Cond.cpp\
Definition.cpp\
Environment.cpp\
Error.cpp\
Eval.cpp\
Frame.cpp\
Lambda.cpp\
Let.cpp\
LetAsterisk.cpp\
Macro.cpp\
MacroFilter.cpp\
Number.cpp\
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
main.cpp\
scheme.cpp

# TEST_TARGET  = ./test$(EXEEXT)
# TEST_SOURCES = test.cpp MessageUtilTest.cpp MessageUtil.cpp

CXXFLAGS = -pg -I . -I /usr/local/include/gc -fno-exceptions -fno-rtti -O0 -gstabs+ -Wall -DGC_OPERATOR_NEW_ARRAY# -DUSE_BOEHM_GC

OBJECTS = $(SOURCES:.cpp=.o)
# TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)

all : $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# check : $(TEST_TARGET)
# 	$(TEST_TARGET)

# $(TEST_TARGET) : $(TEST_OBJECTS)
# 	$(CXX) -lcppunit $(TEST_OBJECTS) /usr/local/lib/libcppunit.a -o $(TEST_TARGET)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies
# 	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(TEST_SOURCES) >> dependencies

clean :
	rm -f $(OBJECTS) $(TARGET) $(TEST_OBJECTS) $(TEST_TARGET)

-include dependencies
