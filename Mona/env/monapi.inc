LFLAGS  = -n -Ttext 0xA0000000 -e _user_start -Bstatic
OFLAGS  = --output-target=elf32-i386
OBJECTS = $(SOURCES:.cpp=.o)

include $(MONADIR)/env/dirnames.inc
include $(MONADIR)/env/Makefile.inc
