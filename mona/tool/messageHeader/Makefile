EXEEXT  = .exe
TARGET  = ./message2header$(EXEEXT)
SOURCES = main.cpp MessageUtil.cpp

TEST_TARGET  = ./test$(EXEEXT)
TEST_SOURCES = test.cpp MessageUtilTest.cpp MessageUtil.cpp

CXXFLAGS = -Wall -g

OBJECTS = $(SOURCES:.cpp=.o)
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)

all : $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

check : $(TEST_TARGET)
	$(TEST_TARGET)

$(TEST_TARGET) : $(TEST_OBJECTS)
	$(CXX) -lcppunit $(TEST_OBJECTS) /usr/local/lib/libcppunit.a -o $(TEST_TARGET)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(TEST_SOURCES) >> dependencies

clean :
	rm -f $(OBJECTS) $(TARGET) $(TEST_OBJECTS) $(TEST_TARGET)

-include dependencies
