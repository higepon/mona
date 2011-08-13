include $(SHAREDIR)/configs/Makefile.inc
include $(SHAREDIR)/configs/dirnames.inc

ifeq ($(IMPSFX),-imp)
LFLAGS    = $(MONAELF_LDS) -n --image-base=0xa0000000 --disable-runtime-pseudo-reloc -e $(USER_START_FUNCTION)
IMPORTFLAGS = --enable-auto-import
else
LFLAGS    = $(MONAELF_LDS) -n -Ttext 0xA0000000 -e $(USER_START_FUNCTION) -Bstatic
OFLAGS    = --output-target=elf32-i386
endif
#mumurik
ifeq ($(OBJECTS),)
CPPOBJECTS = $(SOURCES:.cpp=.o)
CPPOBJECTS2 = $(patsubst %.cc,%.o, $(CPPOBJECTS))
OBJECTS   = $(CPPOBJECTS2) $(CSOURCES:.c=.o) $(NASMSRCS:.asm=.o) $(JSOURCES:.java=.o)
endif
INCLUDE   = -I$(INCDIR) -I$(INCDIR)/monalibc -I$(MONADIR)/tool/STLport-5.2.1/stlport -I$(INCDIR)/c++ $(ADDINCLUDE)
LINK      = $(ADDLINK) -lmonapi$(IMPSFX) $(IMPORTFLAGS)
LINKDEP   = $(LIBDIR)/monapi_crt.o $(LIBDIR)/libmonapi$(IMPSFX).a $(ADDLINKDEP)
 
