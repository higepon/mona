TARGET  = GTETRIS
SOURCES = gtetris.cpp block.cpp tetromino.cpp field.cpp

INSTDIR = $(BINDIR)/APPS/BAYGUI/GTETRIS.APP
INSTFILES = BLOCK0.BM5 BLOCK1.BM5 BLOCK2.BM5 BLOCK3.BM5 BLOCK4.BM5 BLOCK5.BM5 BLOCK6.BM5 MAIN.BM5
CLEANFILES = $(INSTFILES)
$(TARGET).EX5: $(INSTFILES)
SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lbaygui-imp -lmonalibc-imp
ADDLINKDEP = $(MONADIR)/lib/libbaygui-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lbaygui-imp -lmonalibc-imp -lmonapi-imp
ADDLINKDEP = $(MONADIR)/lib/libbaygui.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG
INCLUDE += -I .

BLOCK0.BM5: block0.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0
BLOCK1.BM5: block1.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0
BLOCK2.BM5: block2.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0
BLOCK3.BM5: block3.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0
BLOCK4.BM5: block4.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0
BLOCK5.BM5: block5.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0
BLOCK6.BM5: block6.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0
MAIN.BM5: main.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

-include dependencies
