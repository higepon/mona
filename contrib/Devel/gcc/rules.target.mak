ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../../../mona; pwd)
endif

SHAREDIR   = $(MONADIR)/share
include $(SHAREDIR)/configs/monapi.inc

GCC = $(CC)
MAKE = make
LIBRARIAN = $(WINE) ../toolstdc/golib00.exe
DEL = rm -f

MAKER = $(MAKE) -r
MAKEC = $(MAKE) -C
COMPILE = $(GCC) -c $(CFLAGS)
LINK = $(LD) --Map $@.map $(LFLAGS) $(LIBDIR)/monapi_crt.o 
LINK_ADDLIB = -L$(LIBDIR) -lmonapi-imp -lmonalibc-imp --enable-auto-import

GODRV = ../drv_stdc/drv_stdc.obj
GODRVDIR = ../drv_stdc

# include GO's header files
%.o : %.c makefile ../include/rules.mak
	$(COMPILE) -I ../include -DIN_GCC -DHAVE_CONFIG_H -o $*.o $*.c

# include Mona's header files only
%.obj : %.c makefile ../include/rules.mak
	$(COMPILE) -I $(INCDIR) -I $(INCDIR)/monalibc -o $*.obj $*.c
