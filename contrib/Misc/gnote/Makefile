ifndef $(MONADIR)
export MONADIR=$(PWD)/../../../mona
endif


TARGET  = GNOTE
SOURCES = \
trash/tmenuitem.cpp \
trash/tmenu.cpp \
trash/tmenubar.cpp \
trash/tpopupmenu.cpp \
controller.cpp \
canvas.cpp \
document.cpp \
rootwindow.cpp \
gnote.cpp

INSTDIR = $(BINDIR)/APPS/MONAGUI
INSTFILES = GNOTE.map
SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lamonagui-imp -lmonalibc-imp --enable-auto-import
ADDLINKDEP = $(MONADIR)/lib/libagui-imp.a $(MONADIR)/lib/libmonalibc-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lamonagui-imp -lmonalibc-imp -lmonapi-imp
ADDLINKDEP = $(MONADIR)/lib/libamonagui.a $(MONADIR)/lib/libmonalibc.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG
INCLUDE += -I .

-include dependencies
