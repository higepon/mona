include ../Makefile.inc
BIN             = ../../../bin
CFINCLUDE       = -I. -I../include -I../include/userlib -I../../share
LIB             = -L../lib
LIB_USER        = ../lib/libuser.a
LFLAGS          = -n -Ttext 0xA0000000 -e _user_start -static -luser
OFLAGS          = --output-target=elf32-i386
APP             = $(BIN)/MAP.SVR
OBJ             = MapServer.o

all : $(APP)

$(APP) : $(LIB_USER) $(OBJ)
	$(LD)  $(LIB)  $(OBJ) $(LFLAGS) -o $(APP)
	objcopy $(OFLAGS) $(APP)
	$(STRIP) $(APP)

$(OBJ) : MapServer.cpp
	$(CXX) $(CXXFLAGS) $(CFINCLUDE) -c MapServer.cpp

clean :
	rm -f $(OBJ) $(APP)

