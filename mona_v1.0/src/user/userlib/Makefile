include ../Makefile.inc
CFINCLUDE       = -I../include -I../include/userlib -I../../share
LIB_USER        = ../lib/libuser.a
SOURCES         = string.cpp        \
                  userlib.cpp       \
                  MemoryManager.cpp \

OBJECTS = $(SOURCES:.cpp=.o)

all : $(LIB_USER)

$(LIB_USER) : userlib.o string.o MemoryManager.o
	ar cvr $(LIB_USER) userlib.o string.o MemoryManager.o

.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) $(CFINCLUDE) -c $<

clean :
	rm -f $(LIB_USER) userlib.o string.o MemoryManager.o

depend:
	$(CC) -MM $(CFINCLUDE) $(CFLAGS) $(SOURCES) > dependencies

include dependencies
