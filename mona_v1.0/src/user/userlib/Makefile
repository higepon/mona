include ../Makefile.inc
CFINCLUDE       = -I../include -I../include/userlib -I../../share
LIB_USER        = ../lib/libuser.a
SOURCES         = string.cpp        \
                  userlib.cpp       \
                  MemoryManager.cpp \
                  monalibc.cpp      \
                  MlcCtype.cpp      \
                  MlcStdlib.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all : $(LIB_USER)

$(LIB_USER) : userlib.o string.o MemoryManager.o monalibc.o MlcCtype.o MlcStdlib.o
	ar cvr $(LIB_USER) userlib.o string.o MemoryManager.o monalibc.o MlcCtype.o MlcStdlib.o

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(CFINCLUDE) -c $<

clean :
	rm -f $(LIB_USER) userlib.o string.o MemoryManager.o monalibc.o MlcCtype.o MlcStdlib.o

depend:
	$(CXX) -MM $(CFINCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

include dependencies
