ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../../../mona; pwd)
endif

SHAREDIR   = $(MONADIR)/share
include $(SHAREDIR)/configs/Makefile.inc

GCC = $(CC)
MAKE = make
LIBRARIAN = $(WINE) ../toolstdc/golib00.exe
DEL = rm -f

MAKER = $(MAKE) -r
MAKEC = $(MAKE) -C
COMPILE = $(GCC) -c -Os
LINK = $(GCC) -Wl,-s
LINK_ADDLIB =

GODRV = ../drv_stdc/drv_stdc.obj
GODRVDIR = ../drv_stdc
LINKOPT_CPP0 =
LINKOPT_CC1  =
LINKOPT_CC1P =

%.o : %.c makefile ../include/rules.mak
	$(COMPILE) -DIN_GCC -DHAVE_CONFIG_H -I../include -o $*.o $*.c

%.obj : %.c makefile ../include/rules.mak
	$(COMPILE) -o $*.obj $*.c
