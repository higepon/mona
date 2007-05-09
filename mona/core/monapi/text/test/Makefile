TARGET   = text_test
SOURCES  = \
main.cpp \
LineTest.cpp \
../Line.cpp \
../../CString.cpp \

MONA_INCLUDE_DIR = ../../../../include/
CXXFLAGS = -pg -g -idirafter . -idirafter $(MONA_INCLUDE_DIR)/monapi -I $(MONA_INCLUDE_DIR) -Wall -D TEXT_TEST
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
