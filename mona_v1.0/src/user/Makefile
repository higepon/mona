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
USER_PROCESS    = $(BIN)/USER.ELF
USER_PROCESS2   = $(BIN)/TEST.ELF
KEYBOARD_SERVER = $(BIN)/KEYBDMNG.SVR
SOURCES         = hello.cpp           \
                  test.cpp            \
                  userlib.cpp         \
                  KeyBoardManager.cpp \
                  string.cpp          \
                  KeyBoardServer.cpp  \
                  MemoryManager.o     \

OBJECTS = $(SOURCES:.cpp=.o)

all : $(USER_PROCESS) $(USER_PROCESS2) $(KEYBOARD_SERVER)

$(USER_PROCESS) : userlib.o KeyBoardManager.o hello.o MemoryManager.o string.o
	ld -n -Ttext 0xA0000000 -e _user_start -static hello.o userlib.o MemoryManager.o string.o -o $(USER_PROCESS)
	objcopy --output-target=elf32-i386 $(USER_PROCESS)

$(USER_PROCESS2) : userlib.o KeyBoardManager.o test.o MemoryManager.o string.o
	ld -n -Ttext 0xA0000000 -e _user_start -static test.o userlib.o MemoryManager.o string.o -o $(USER_PROCESS2)
	objcopy --output-target=elf32-i386 $(USER_PROCESS2)

$(KEYBOARD_SERVER) : userlib.o KeyBoardManager.o MemoryManager.o string.o KeyBoardServer.o
	ld -n -Ttext 0xA0000000 -e _user_start -static KeyBoardServer.o userlib.o MemoryManager.o KeyBoardManager.o string.o -o $(KEYBOARD_SERVER)
	objcopy --output-target=elf32-i386 $(KEYBOARD_SERVER)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) $(CFINCLUDE) -c $<

depend:
	$(CC) -MM $(CFINCLUDE) $(CFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(USER_PROCESS) $(USER_PROCESS2)

include dependencies
