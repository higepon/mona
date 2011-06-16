EXEEXT  = .exe
TARGET  = ./test$(EXEEXT)
SOURCES = test.cpp String.cpp Object.cpp

INCLUDE = -I .  -I ../ -I ../../../../mona/include
CXXFLAGS = -Wall -g

OBJECTS = $(SOURCES:.cpp=.o)

all : symlink $(TARGET)

symlink:
	ln -sf ../lang/String.cpp String.cpp
	ln -sf ../lang/Object.cpp Object.cpp

check: all
	valgrind --leak-check=full ./test.exe


$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(TARGET)

check-syntax:
	$(CXX) -o nul $(CXXFLAGS) $(INCLUDES) -S ${CHK_SOURCES}


-include dependencies
