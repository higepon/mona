include ../Makefile.inc
BIN             = ../../../bin
CFINCLUDE       = -I. -I../include -I../include/userlib -I../../share
LIB             = -L../lib
LIB_USER        = ../lib/libuser.a
LFLAGS          = -n -Ttext 0xA0000000 -e _user_start -static -luser
OFLAGS          = --output-target=elf32-i386
APP             = $(BIN)/SHELL.SVR
OBJ             = ShellServer.o

all : $(APP)

$(APP) : $(LIB_USER) $(OBJ) KeyBoardManager.o
	$(LD) --Map app.map $(LFLAGS) $(OBJ) KeyBoardManager.o $(LIB) -o $(APP)
	objcopy $(OFLAGS) $(APP)
	$(STRIP) $(APP)

$(OBJ) : ShellServer.cpp Shell.h
	$(CXX) $(CXXFLAGS) $(CFINCLUDE) -c ShellServer.cpp

KeyBoardManager.o : KeyBoardManager.cpp KeyBoardManager.h
	$(CXX) $(CXXFLAGS) $(CFINCLUDE) -c KeyBoardManager.cpp

clean :
	rm -f $(OBJ) $(APP) KeyBoardManager.o

