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
	$(LD)  $(LIB)  $(OBJ) jpegls.o $(LFLAGS) -o $(APP)
	objcopy $(OFLAGS) $(APP)
	$(STRIP) $(APP)

$(OBJ) : jpegdemo.cpp
	$(CXX) $(CXXFLAGS) $(CFINCLUDE) -c jpegdemo.cpp

jpegls.o : jpegls.cpp jpegls.h text.hex dct_tab
	$(CXX) $(CXXFLAGS) $(CFINCLUDE) -c jpegls.cpp

clean :
	rm -f $(OBJ) $(APP) jpegls.o

