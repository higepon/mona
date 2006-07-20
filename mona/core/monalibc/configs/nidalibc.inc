include $(SHAREDIR)/configs/Makefile.inc
include $(SHAREDIR)/configs/dirnames.inc

ifeq ($(IMPSFX),-imp)
LFLAGS    = $(MONAELF_LDS) -n --image-base=0xa0000000 -e $(USER_START_FUNCTION)
else
LFLAGS    = $(MONAELF_LDS) -n -Ttext 0xA0000000 -e $(USER_START_FUNCTION) -Bstatic
OFLAGS    = --output-target=elf32-i386
endif
OBJECTS   = $(SOURCES:.cpp=.o) $(CSOURCES:.c=.o) $(NASMSRCS:.asm=.o)
INCLUDE   = -I$(INCDIR)
LINK      = $(ADDLINK) -lmonapi$(IMPSFX)
LINKDEP   = $(LIBDIR)/crt.o $(LIBDIR)/libmonapi$(IMPSFX).a $(ADDLINKDEP)
