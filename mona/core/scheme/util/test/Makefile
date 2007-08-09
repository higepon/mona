TARGET   = util_test
SOURCES  = main.cpp StringTest.cpp VectorTest.cpp
CXXFLAGS = -pg -g -idirafter . -Wall
CFLAGS   = $(CXXFLAGS)

OBJECTS = $(SOURCES:.cpp=.o)
all : $(TARGET)

$(TARGET):  $(OBJECTS)
	$(CXX) -lcppunit -ldl -g $(OBJECTS) -o $(TARGET) $(LDFLAGS)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -g $(CXXFLAGS) $(INCLUDE) -c $< -o $@

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(TARGET)

check : $(TARGET)
	@./$<

-include dependencies
