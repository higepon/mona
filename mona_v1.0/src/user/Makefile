NASM       = nasm
NDISASM    = ndisasm
CC         = /usr/bin/g++
LD         = /usr/bin/ld
STRIP      = /usr/bin/strip
CONTRIB_LIB= ./contrib/lib_cygwin
LFLAGS     = -n -Ttext 0x1200 -static
# for gcc3.x@cygwin
CFLAGS     = -nostdlib -fno-exceptions  -fno-rtti -Wall
ENV             = env
BIN             = ../../bin
USERINCLUDE     = include
CFINCLUDE       = -I$(USERINCLUDE)
HELLO           = $(BIN)/HELLO.ELF
KEYBOARD_SERVER = $(BIN)/KEYBDMNG.SVR
SHELL_SERVER    = $(BIN)/SHELL.SVR
JPEG_DEMO       = $(BIN)/JPEGDEMO.ELF
MAP_SERVER      = $(BIN)/MAP.SVR
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

OBJECTS = $(SOURCES:.cpp=.o)

all : $(HELLO) $(KEYBOARD_SERVER) $(SHELL_SERVER) $(JPEG_DEMO) $(MAP_SERVER)

$(HELLO) : userlib.o hello.o MemoryManager.o string.o
	ld -n -Ttext 0xA0000000 -e _user_start -static hello.o userlib.o MemoryManager.o string.o -o $(HELLO)
	objcopy --output-target=elf32-i386 $(HELLO)

$(KEYBOARD_SERVER) : userlib.o KeyBoardManager.o MemoryManager.o string.o KeyBoardServer.o
	ld -n -Ttext 0xA0000000 -e _user_start -static KeyBoardServer.o userlib.o MemoryManager.o KeyBoardManager.o string.o -o $(KEYBOARD_SERVER)
	objcopy --output-target=elf32-i386 $(KEYBOARD_SERVER)

$(SHELL_SERVER) : userlib.o MemoryManager.o string.o ShellServer.o KeyBoardManager.o
	ld -n -Ttext 0xA0000000 -e _user_start -static ShellServer.o userlib.o KeyBoardManager.o MemoryManager.o string.o  -o $(SHELL_SERVER)
	objcopy --output-target=elf32-i386 $(SHELL_SERVER)

$(JPEG_DEMO) : userlib.o MemoryManager.o string.o jpegdemo.o jpegls.o
	ld -n -Ttext 0xA0000000 -e _user_start -static jpegdemo.o userlib.o jpegls.o MemoryManager.o string.o  -o $(JPEG_DEMO)
	objcopy --output-target=elf32-i386 $(JPEG_DEMO)

$(MAP_SERVER) : MapServer.o userlib.o MemoryManager.o string.o
	ld -n -Ttext 0xA0000000 -e _user_start -static MapServer.o userlib.o MemoryManager.o string.o  -o $(MAP_SERVER)
	objcopy --output-target=elf32-i386 $(MAP_SERVER)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) $(CFINCLUDE) -c $<

depend:
	$(CC) -MM $(CFINCLUDE) $(CFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(HELLO) $(KEYBOARD_SERVER) $(SHELL_SERVER) $(JPEG_DEMO) $(MAP_SERVER)

include dependencies
