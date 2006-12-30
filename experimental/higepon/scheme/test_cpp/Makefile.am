bin_PROGRAMS = test_cpp
test_cpp_CXXFLAGS = -g -Wall -O3 -I../src -I /usr/local/include/gc -DGC_OPERATOR_NEW_ARRAY -DUSE_BOEHM_GC -O0 -gstabs+ -Wall
test_cpp_LDFLAGS  = -lcppunit -lgc
test_cpp_SOURCES = main.cpp\
MacroMatchTest.h\
MacroMatchTest.cpp\
../src/Tokenizer.cpp\
../src/Tokenizer.h\
../src/Node.h\
../src/Node.cpp\
../src/MacroUtility.h\
../src/MacroUtility.cpp\
../src/Parser.h\
../src/Parser.cpp
