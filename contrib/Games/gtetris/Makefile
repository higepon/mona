TARGET  = GTETRIS
SOURCES = gtetris.cpp block.cpp tetromino.cpp field.cpp

INSTDIR = $(BINDIR)/APPS/MONAGUI/GTETRIS.APP
INSTFILES = block0.bmp block1.bmp block2.bmp block3.bmp block4.bmp block5.bmp block6.bmp main.bmp
CLEANFILES =
$(TARGET).EX5: $(INSTFILES)
SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lamonagui-imp -lmonalibc-imp --enable-auto-import
ADDLINKDEP = $(MONADIR)/lib/libamonagui-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lamonagui-imp -lmonalibc-imp -lmonapi-imp
ADDLINKDEP = $(MONADIR)/lib/libamonagui.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG
INCLUDE += -I .

-include dependencies
