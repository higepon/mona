TARGET        = ./scheme
TEST_TARGET   = ./scheme_test
TARGET_LIBS   = lib/interact.scm lib/batch.scm lib/danny.scm lib/core.scm lib/shell.scm lib/unittest.scm lib/conf.scm lib/demo.scm
TEST_FILES  = test/char.scm test/closure.scm test/cont.scm test/generate.scm \
              test/number.scm test/pair.scm test/port.scm test/string.scm test/symbol.scm \
              test/test.scm test/util.scm test/vector.scm test/dir.scm

# -O3 では gc がうごかなかった => 多重継承時にキャストでポインタの値がかわるのが原因。
CXXFLAGS = -O3 -g -idirafter . -Wall $(GC_CFLAGS) -I ./onig/onig-5.7.0 # -finstrument-functions #-DMACRO_TRACE -pg 
CFLAGS   = $(CXXFLAGS)
LDFLAGS  = $(GC_LDFLAGS)
NFLAGS   = -felf
include Makefile.src.inc

# 動作がおかしかったら
# gc を OFFに
# 最適化をやめる

include Makefile.boehm_gc.inc
#include Makefile.mona_gc.inc
# たまに これで make checkする
#include Makefile.none_gc.inc

include Makefile.onig.inc

TEST_SOURCES = \
test_cpp/yaml.cpp\
test_cpp/ScannerTest.cpp\
test_cpp/ParserTest.cpp

OBJECTS = $(SOURCES:.cpp=.o)
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)

ONIG_OBJECTS = $(ONIG_SOURCES:.c=.o)
all : $(TARGET) $(TEST_TARGET)

include Makefile.dep.inc

$(TARGET):  $(ONIG_DIR) $(BOEHM_GC_DIR) $(REGISTERS_INC) $(PROCEDURES_H) main.o $(OBJECTS) $(LIBCONT_OBJECTS) $(GC_OBJECTS) $(ONIG_OBJECTS) $(TARGET_LIBS) $(TEST_FILES)
	$(CXX) -pg -g main.o $(OBJECTS) $(LIBCONT_OBJECTS) $(GC_OBJECTS) $(ONIG_OBJECTS) -o $(TARGET) $(LDFLAGS)

$(TEST_TARGET):  $(REGISTERS_INC) $(PROCEDURES_H) test_cpp/main.o $(OBJECTS) $(LIBCONT_OBJECTS) $(TEST_OBJECTS) $(GC_OBJECTS) $(ONIG_OBJECTS)
	$(CXX) -lcppunit -ldl -lonig -g test_cpp/main.o $(OBJECTS) $(LIBCONT_OBJECTS) $(GC_OBJECTS) $(ONIG_OBJECTS) $(TEST_OBJECTS) -o $(TEST_TARGET) $(LDFLAGS)

check : $(TEST_TARGET) $(TARGET)
	$(TEST_TARGET)
	$(TARGET) test/test.scm

gdb : $(TARGET)
	gdb -x ./gdb.txt $(TARGET)

test_cpp/main.o: test_cpp/main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(ONIG_DIR) :
	(cd onig && $(MAKE))

$(BOEHM_GC_DIR) :
	(cd BoehmGC && $(MAKE))

.SUFFIXES: .scm.in .scm
.scm.in.scm:
	sed -e 's|<LIBDIR>|./lib|g' $< > $@
	sed -e 's|<IF_MONA>|;|g' $< | sed -e 's|<IF_LINUX>||g' | sed -e 's|<LIBDIR>|./lib|g' | sed -e 's|<TESTDIR>|./test|g' > $@

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
	rm -f $(OBJECTS) $(TARGET) main.o $(TEST_OBJECTS) $(LIBCONT_OBJECTS) $(TEST_TARGET) \
$(ONIG_OBJECTS) $(GC_OBJECTS) $(REGISTERS_INC) $(PROCEDURES_H) $(TARGET_LIBS) test/test.scm test/dir.scm test/port.scm
#	(cd onig && $(MAKE) $@)

-include dependencies
