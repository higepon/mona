TARGET        = ./scheme
TEST_TARGET   = ./scheme_test
TARGET_LIBS   = lib/MONA.SCM lib/DANNY.SCM

CXXFLAGS = -pg -g -idirafter . -Wall $(GC_CFLAGS) #-DMACRO_TRACE
CFLAGS   = $(CXXFLAGS)
LDFLAGS  = $(GC_LDFLAGS)
NFLAGS   = -felf
include Makefile.src.inc

#include Makefile.boehm_gc.inc
include Makefile.mona_gc.inc
# たまに これで make checkする
#include Makefile.none_gc.inc

TEST_SOURCES = \
test_cpp/yaml.cpp\
test_cpp/MacroMatchTest.cpp\
test_cpp/MacroFilterTest.cpp\
test_cpp/QuoteFilterTest.cpp\
test_cpp/ScannerTest.cpp\
test_cpp/ExtRepParserTest.cpp

OBJECTS = $(SOURCES:.cpp=.o)
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)

GC_OBJECTS = $(GC_SOURCES:.cpp=.o)
all : $(TARGET) $(TEST_TARGET)

include Makefile.dep.inc

$(TARGET):  $(REGISTERS_INC) $(PROCEDURES_H) main.o $(OBJECTS) $(LIBCONT_OBJECTS) $(GC_OBJECTS) $(TARGET_LIBS)
	$(CXX) -pg -g main.o $(OBJECTS) $(LIBCONT_OBJECTS) $(GC_OBJECTS) -o $(TARGET) $(LDFLAGS)

$(TEST_TARGET):  $(REGISTERS_INC) $(PROCEDURES_H) test_cpp/main.o $(OBJECTS) $(LIBCONT_OBJECTS) $(TEST_OBJECTS) $(GC_OBJECTS)
	$(CXX) -lcppunit -ldl -g test_cpp/main.o $(OBJECTS) $(LIBCONT_OBJECTS) $(GC_OBJECTS) $(TEST_OBJECTS) -o $(TEST_TARGET) $(LDFLAGS)

check : $(TEST_TARGET) $(TARGET)
	$(TEST_TARGET)
	$(TARGET) test/test.scm

gdb : $(TARGET)
	gdb -x ./gdb.txt $(TARGET)

test_cpp/main.o: test_cpp/main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

.SUFFIXES: .SCM.in .SCM
.SCM.in.SCM:
	sed -e 's|<LIBDIR>|./lib|g' $< > $@

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -g $(CXXFLAGS) $(INCLUDE) -c $< -o $@

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(TEST_SOURCES) >> dependencies

clean :
	rm -f $(OBJECTS) $(TARGET) main.o $(TEST_OBJECTS) $(LIBCONT_OBJECTS) $(TEST_TARGET) $(GC_OBJECTS) $(REGISTERS_INC) $(PROCEDURES_H) $(TARGET_LIBS)

-include dependencies
