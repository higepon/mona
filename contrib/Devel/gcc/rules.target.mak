ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../../../mona; pwd)
endif

#ADDLINK    = -lmonalibc-imp
SHAREDIR   = $(MONADIR)/share
include $(SHAREDIR)/configs/monapi.inc

# GCC = /usr/bin/i586-mingw32msvc-gcc -nostdinc -I . -idirafter $(MONADIR)/include  -idirafter $(MONADIR)/include/monalibc #-DSTDARG_H=1 -fno-exceptions -fno-rtti -fno-strict-aliasing -fno-builtin
# MAKE = make
# LIBRARIAN = ../toolstdc/golib00.exe
# DEL = rm -f

# MAKER = $(MAKE) -r
# MAKEC = $(MAKE) -C
# COMPILE = $(GCC) -c -Os -fno-exceptions -fno-rtti -fno-strict-aliasing -fno-builtin -nostdlib
# LINK = $(GCC) -Wl,-s -fno-exceptions -fno-rtti -fno-strict-aliasing -fno-buitin -nostdlib $(MONADIR)/lib/monapi_crt.o
# LINK_ADDLIB = -L $(MONADIR)/lib -lmonalibc-imp -lmonapi-imp

# GODRV = ../drv_stdc/drv_stdc.obj
# GODRVDIR = ../drv_stdc
# LINKOPT_CPP0 = -Ttext 0xA0000000 -e _user_start -Bstatic
# LINKOPT_CC1  = -Ttext 0xA0000000 -e _user_start -Bstatic
# LINKOPT_CC1P = -Ttext 0xA0000000 -e _user_start -Bstatic

# %.o : %.c makefile ../include/rules.mak
# 	echo "hoge"
# 	$(COMPILE) -DIN_GCC -DHAVE_CONFIG_H -I../include -o $*.o $*.c

# %.obj : %.c makefile ../include/rules.mak
# 	$(COMPILE) -o $*.obj $*.c
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

# include GO'S header files
%.o : %.c makefile ../include/rules.mak
	$(COMPILE) -I ../include -DIN_GCC -DHAVE_CONFIG_H -o $*.o $*.c

# include Mona's header files only
%.obj : %.c makefile ../include/rules.mak
	$(COMPILE) -I $(INCDIR) -I $(INCDIR)/monalibc -o $*.obj $*.c
