NASM       = nasm
NDISASM    = ndisasm
CC         = /usr/bin/g++
LD         = /usr/bin/ld
STRIP      = /usr/bin/strip
CONTRIB_LIB= ./contrib/lib_cygwin
LFLAGS     = -n -Ttext 0x1200 -static
# for gcc3.x@cygwin
CFLAGS     = -nostdlib -fno-exceptions  -fno-rtti -Wall -O3
ENV             = env
BIN             = ../../bin
USERINCLUDE     = include
CFINCLUDE       = -I$(USERINCLUDE)
HELLO           = $(BIN)/HELLO.ELF
KEYBOARD_SERVER = $(BIN)/KEYBDMNG.SVR
SHELL_SERVER    = $(BIN)/SHELL.SVR
JPEG_DEMO       = $(BIN)/JPEGDEMO.ELF
MAP_SERVER      = $(BIN)/MAP.SVR
MOUSE_SERVER    = $(BIN)/MOUSE.SVR
NOIZ2BG         = $(BIN)/NOIZ2BG.ELF
CLOCK           = $(BIN)/CLOCK.ELF
LIB             = -L./lib
LIB_USER        = lib/libuser.a
LFLAGS          = -n -Ttext 0xA0000000 -e _user_start -static -luser
OFLAGS          = --output-target=elf32-i386
SOURCES         = hello.cpp                   \
                  test.cpp                    \
                  userlib.cpp                 \
                  KeyBoardManager.cpp         \
                  string.cpp                  \
                  KeyBoardServer.cpp          \
                  MemoryManager.cpp           \
                  UFAT12.cpp                  \
                  BitMap.cpp                  \
                  VirtualFileSystemServer.cpp \
                  jpegls.cpp                  \
                  ShellServer.cpp             \
                  jpegdemo.cpp                \
                  MapServer.cpp               \
                  MouseServer.cpp             \
                  noiz2bg.cpp                 \
                  clock.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all : $(HELLO) $(KEYBOARD_SERVER) $(SHELL_SERVER) $(JPEG_DEMO) $(MAP_SERVER) $(MOUSE_SERVER)  $(NOIZ2BG) $(CLOCK)

$(LIB_USER) : userlib.o string.o MemoryManager.o
	ar cvr $(LIB_USER) userlib.o string.o MemoryManager.o

$(HELLO) : hello.o $(LIB_USER)
	$(LD) $(LFLAGS) hello.o $(LIB) -o $(HELLO)
	objcopy $(OFLAGS) $(HELLO)

$(CLOCK) : clock.o $(LIB_USER)
	$(LD) $(LFLAGS) clock.o $(LIB) -o $(CLOCK)
	objcopy $(OFLAGS) $(CLOCK)

$(KEYBOARD_SERVER) : KeyBoardManager.o KeyBoardServer.o $(LIB_USER)
	$(LD) $(LFLAGS) KeyBoardServer.o KeyBoardManager.o $(LIB) -o $(KEYBOARD_SERVER)
	objcopy $(OFLAGS) $(KEYBOARD_SERVER)

$(MOUSE_SERVER) : $(LIB_USER) MouseServer.o
	$(LD) $(LFLAGS) MouseServer.o $(LIB) -o $(MOUSE_SERVER)
	objcopy $(OFLAGS) $(MOUSE_SERVER)

$(SHELL_SERVER) : ShellServer.o KeyBoardManager.o $(LIB_USER)
	$(LD) $(LFLAGS) ShellServer.o KeyBoardManager.o $(LIB) -o $(SHELL_SERVER)
	objcopy $(OFLAGS) $(SHELL_SERVER)

$(JPEG_DEMO) : jpegdemo.o jpegls.o $(LIB_USER)
	$(LD) $(LFLAGS) jpegdemo.o jpegls.o $(LIB)  -o $(JPEG_DEMO)
	objcopy $(OFLAGS) $(JPEG_DEMO)

$(MAP_SERVER) : MapServer.o $(LIB_USER)
	$(LD) $(LFLAGS) MapServer.o $(LIB) -o $(MAP_SERVER)
	objcopy $(OFLAGS) $(MAP_SERVER)

$(NOIZ2BG) : noiz2bg.o $(LIB_USER)
	$(LD) $(LFLAGS) noiz2bg.o $(LIB) -o $(NOIZ2BG)
	objcopy $(OFLAGS) $(NOIZ2BG)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) $(CFINCLUDE) -c $<

depend:
	$(CC) -MM $(CFINCLUDE) $(CFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(HELLO) $(KEYBOARD_SERVER) $(SHELL_SERVER) $(JPEG_DEMO) $(MAP_SERVER) $(LIB_USER) $(MOUSE_SERVER) $(NOIZ2BG) $(CLOCK)

include dependencies
