include ../Makefile.inc
CFINCLUDE       = -I../include -I../include/userlib -I../../share
LIB_USER        = ../lib/libuser.a
SOURCES         = string.cpp        \
                  userlib.cpp       \
                  MemoryManager.cpp \
                  monalibc.cpp      \
                  MlcCtype.cpp      \
                  MlcStdlib.cpp     \
                  MlcStdio.cpp      \
                  MlcStdarg.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all : $(LIB_USER)

$(LIB_USER) : $(OBJECTS)
	ar cvr $@ $(OBJECTS)
	ranlib $@

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(CFINCLUDE) -c $<

clean :
	rm -f $(LIB_USER) $(OBJECTS)

depend:
	$(CXX) -MM $(CFINCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

include dependencies
