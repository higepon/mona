TARGET = $(SRCS:.c=.exe)

IMPSFX = -imp
include $(MONADIR)/env/monapi.inc

CRT_OBJECT = $(TOP)/../tmpmonalibc/crt.o
CFLAGS    += -nostdinc -DGLUT_IMPORT_LIB
CXXFLAGS  += -nostdinc -DGLUT_IMPORT_LIB -DDEBUG
INCLUDE   += -I$(TOP)/include -I$(TOP)/../Mesa-6.2.1/include -I$(TOP)/../tmpmonalibc
ADDLINK    = -L$(TOP)/../Mesa-6.2.1/lib -L$(TOP)/glut -L$(TOP)/../tmpmonalibc -lglut-imp -lGLU-imp -lOSMesa-imp -ltmpmonalibc-imp -lmonalibc-imp

all: $(TARGET)

.SUFFIXES: .c .exe
.c.exe:
	$(CC) -c $(CFLAGS) $(INCLUDE) $<
	$(LD) -s --Map $(@:.exe=.map) $(LFLAGS) -o $@ $(CRT_OBJECT) $(<:.c=.o) -L$(LIBDIR) $(LINK)

clean:
	rm -f *.exe *.o *.map
