include $(MONADIR)/env/dirnames.inc
include $(MONADIR)/env/Makefile.inc

LFLAGS    = $(MONAELF_LDS) -n -Ttext 0xA0000000 -e $(USER_START_FUNCTION) -Bstatic
OFLAGS    = --output-target=elf32-i386
OBJECTS   = $(SOURCES:.cpp=.o)
LIBMONAPI = $(LIBDIR)/monapi.o $(LIBDIR)/libmonapi.a

