include ../Makefile.inc
BIN             = ../../../bin
CFINCLUDE       = -I. -I../include -I../include/userlib -I../../share
LIB             = -L../lib
LIB_USER        = ../lib/libuser.a
LFLAGS          = -n -Ttext 0xA0000000 -e _user_start -static -luser
OFLAGS          = --output-target=elf32-i386
APP             = $(BIN)/KEYBDMNG.SVR
OBJ             = KeyBoardServer.o

all : $(APP)

$(APP) : $(LIB_USER) $(OBJ) KeyBoardManager.o
	$(LD) $(LFLAGS) $(OBJ) KeyBoardManager.o $(LIB) -o $(APP)
	objcopy $(OFLAGS) $(APP)
	$(STRIP) $(APP)

$(OBJ) : KeyBoardServer.cpp
	$(CXX) $(CFLAGS) $(CFINCLUDE) -c KeyBoardServer.cpp

KeyBoardManager.o : KeyBoardManager.cpp KeyBoardManager.h
	$(CXX) $(CFLAGS) $(CFINCLUDE) -c KeyBoardManager.cpp

clean :
	rm -f $(OBJ) KeyBoardManager.o $(APP)

