include ../Makefile.inc
BIN             = ../../../bin
CFINCLUDE       = -I. -I../include -I../include/userlib -I../../share
LIB             = -L../lib
LIB_USER        = ../lib/libuser.a
LFLAGS          = -n -Ttext 0xA0000000 -e _user_start -static -luser
OFLAGS          = --output-target=elf32-i386
APP             = $(BIN)/JPEGDEMO.ELF
OBJ             = jpegdemo.o

all : $(APP)

$(APP) : $(LIB_USER) $(OBJ) jpegls.o
	$(LD) $(LFLAGS) $(OBJ) jpegls.o $(LIB) -o $(APP)
	objcopy $(OFLAGS) $(APP)
	$(STRIP) $(APP)

$(OBJ) : jpegdemo.cpp
	$(CXX) $(CFLAGS) $(CFINCLUDE) -c jpegdemo.cpp

jpegls.o : jpegls.cpp jpegls.h text.hex dct_tab
	$(CXX) $(CFLAGS) $(CFINCLUDE) -c jpegls.cpp

clean :
	rm -f $(OBJ) $(APP) jpegls.o

