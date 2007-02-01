TARGET  = ./cont

SOURCES = \
main.cpp \
mysetjmp.cpp\
jmp_outside_test.cpp\
jmp_inside_test.cpp


CXXFLAGS = -g -gstabs -idirafter . -Wall -O0

OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET)

$(TARGET): $(OBJECTS) mysetjmp_asm.o
	$(CXX) mysetjmp_asm.o $(OBJECTS) -o $(TARGET)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

mysetjmp_asm.o: mysetjmp.asm
	nasm $< -o $@ -felf

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(TARGET) mysetjmp_asm.o

-include dependencies
