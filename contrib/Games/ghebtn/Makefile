TARGET  = GHEBTN
SOURCES = ghebtn.cpp

INSTDIR = $(BINDIR)/APPS/BAYGUI/GHEBTN.APP
INSTFILES = 0.BM5 1.BM5 2.BM5 3.BM5 4.BM5 5.BM5 6.BM5 7.BM5 8.BM5 9.BM5 HENORMAL.BM5 HEPUSHED.BM5
CLEANFILES = $(INSTFILES)
$(TARGET).EX5: $(INSTFILES)
SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lbaygui-imp -lmonalibc-imp --enable-auto-import
ADDLINKDEP = $(MONADIR)/lib/libbaygui-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lbaygui-imp -lmonalibc-imp -lmonapi-imp
ADDLINKDEP = $(MONADIR)/lib/libbaygui.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG
INCLUDE += -I .

0.BM5: 0.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

1.BM5: 1.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

2.BM5: 2.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

3.BM5: 3.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

4.BM5: 4.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

5.BM5: 5.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

6.BM5: 6.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

7.BM5: 7.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

8.BM5: 8.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

9.BM5: 9.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

HENORMAL.BM5: henormal.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

HEPUSHED.BM5: hepushed.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

-include dependencies
