include ../Makefile.inc
BIN             = ../../../bin
CFINCLUDE       = -I. -I../include -I../include/userlib -I../../share
LIB             = -L../lib
LIB_USER        = ../lib/libuser.a
LFLAGS          = -n -Ttext 0xA0000000 -e _user_start -static -luser
OFLAGS          = --output-target=elf32-i386
APP             = $(BIN)/NOIZ2BG.ELF
OBJ             = noiz2bg.o

all : $(APP)

$(APP) : $(LIB_USER) $(OBJ)
	$(LD) $(LFLAGS) $(OBJ) $(LIB) -o $(APP)
	objcopy $(OFLAGS) $(APP)
	$(STRIP) $(APP)

$(OBJ) : noiz2bg.cpp
	$(CXX) $(CXXFLAGS) $(CFINCLUDE) -c noiz2bg.cpp

clean :
	rm -f $(OBJ) $(APP)

