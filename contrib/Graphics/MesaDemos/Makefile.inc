TARGET = $(SRCS:.c=.EXE)

IMPSFX = -imp
MONADIR = ../../../../mona
include $(MONADIR)/share/configs/Makefile.inc
include $(MONADIR)/share/configs/dirnames.inc
include $(MONADIR)/share/configs/monapi.inc

CRT_OBJECT = $(MONADIR)/lib/monapi_crt.o
CFLAGS    += -nostdinc -DGLUT_IMPORT_LIB
CXXFLAGS  += -nostdinc -DGLUT_IMPORT_LIB -DDEBUG
INCLUDE   += -I$(TOP)/include -I$(TOP)/../Mesa-6.2.1/include
ADDLINK    = -L$(TOP)/../Mesa-6.2.1/lib -L$(TOP)/glut -lglut-imp -lGLU-imp -lOSMesa-imp -lmonalibc-imp

all: $(TARGET)

.SUFFIXES: .c .EXE
.c.EXE:
	$(CC) -c $(CFLAGS) $(INCLUDE) $<
	$(LD) -s --Map $(@:.EXE=.map) $(LFLAGS) -o $@ $(CRT_OBJECT) $(<:.c=.o) -L$(LIBDIR) $(LINK)

clean:
	rm -f *.EXE *.o *.map
