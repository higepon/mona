bin_PROGRAMS = test_cpp
test_cpp_CXXFLAGS = -g -Wall -O3 -I../src -I /usr/local/include/gc -DGC_OPERATOR_NEW_ARRAY -DUSE_BOEHM_GC -O0 -gstabs+ -Wall
test_cpp_LDFLAGS  = -lcppunit -lgc
test_cpp_SOURCES = main.cpp\
yaml.cpp\
yaml.h\
MacroMatchTest.h\
MacroMatchTest.cpp\
MacroTranslateTest.h\
MacroTranslateTest.cpp\
../src/Tokenizer.cpp\
../src/Tokenizer.h\
../src/Node.h\
../src/Node.cpp\
../src/scheme.h\
../src/Macro.h\
../src/Macro.cpp\
../src/Parser.h\
../src/Parser.cpp
