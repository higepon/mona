include ../Makefile.inc
BIN             = ../../../bin
CFINCLUDE       = -I. -I../include -I../include/userlib -I../../share
LIB             = -L../lib
LIB_USER        = ../lib/libuser.a
LFLAGS          = -n -Ttext 0xA0000000 -e _user_start -static -luser
OFLAGS          = --output-target=elf32-i386
APP             = $(BIN)/REVERSI.ELF
OBJ             = reversi.o

all : $(APP)

$(APP) : $(LIB_USER) $(OBJ) ReversiBoard.o
	$(LD) $(LFLAGS) $(OBJ) ReversiBoard.o $(LIB) -o $(APP)
	objcopy $(OFLAGS) $(APP)
	$(STRIP) $(APP)

$(OBJ) : reversi.cpp
	$(CXX) $(CFLAGS) $(CFINCLUDE) -c reversi.cpp

ReversiBoard.o : ReversiBoard.cpp ReversiBoard.h
	$(CXX) $(CFLAGS) $(CFINCLUDE) -c ReversiBoard.cpp

clean :
	rm -f $(OBJ) ReversiBoard.o $(APP)

