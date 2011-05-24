ifndef $(MONADIR)
# export MONADIR=$(PWD)/../mona
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

TARGET  = MONASQ
SOURCES = monasq.cpp \
	monasq_monapi.cpp \
	monasq_monagui.cpp \
	init_script.cpp \
	misc/TextArea.cpp \
	misc/vsprintf.cpp \
	misc/fcvt.cpp \
	sq/squirrel/sqapi.cpp \
	sq/squirrel/sqbaselib.cpp \
	sq/squirrel/sqfuncstate.cpp \
	sq/squirrel/sqdebug.cpp \
	sq/squirrel/sqlexer.cpp \
	sq/squirrel/sqobject.cpp \
	sq/squirrel/sqcompiler.cpp \
	sq/squirrel/sqstate.cpp \
	sq/squirrel/sqtable.cpp \
	sq/squirrel/sqmem.cpp \
	sq/squirrel/sqvm.cpp \
	sq/squirrel/sqclass.cpp \
	sq/sqstdlib/sqstdblob.cpp \
	sq/sqstdlib/sqstdio.cpp \
	sq/sqstdlib/sqstdstream.cpp \
	sq/sqstdlib/sqstdmath.cpp \
	sq/sqstdlib/sqstdsystem.cpp \
	sq/sqstdlib/sqstdstring.cpp \
	sq/sqstdlib/sqstdaux.cpp \
	sq/sqstdlib/sqstdrex.cpp \
	sqplus/SqPlus.cpp \
	sqplus/SquirrelVM.cpp \
	sqplus/SquirrelObject.cpp \
	sqplus/SquirrelBindingsUtils.cpp
#	misc/setjmp.cpp \

INSTDIR = $(BINDIR)/APPS/MONAGUI

SHAREDIR = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lamonagui-imp -lmonalibc-imp --enable-auto-import
ADDLINKDEP = $(MONADIR)/lib/libamonagui-imp.a $(MONADIR)/lib/libmonalibc-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lmonagui -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonagui.a $(MONADIR)/lib/libmonalibc.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DMONASQ -DDEBUG

INCLUDE  += -I./ -I./sq/include -I./sqplus

-include dependencies
