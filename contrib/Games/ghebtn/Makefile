TARGET  = GHEBTN
SOURCES = ghebtn.cpp
BIM2BIN = `which bim2bin`

INSTDIR = $(BINDIR)/APPS/MONAGUI/GHEBTN.APP
INSTFILES = 0.BM5 1.BM5 2.BM5 3.BM5 4.BM5 5.BM5 6.BM5 7.BM5 8.BM5 9.BM5 HENORMAL.BM5 HEPUSHED.BM5
CLEANFILES = $(INSTFILES)
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

0.BM5: 0.bmp
	$(BIM2BIN) in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

1.BM5: 1.bmp
	$(BIM2BIN) in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

2.BM5: 2.bmp
	$(BIM2BIN) in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

3.BM5: 3.bmp
	$(BIM2BIN) in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

4.BM5: 4.bmp
	$(BIM2BIN) in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

5.BM5: 5.bmp
	$(BIM2BIN) in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

6.BM5: 6.bmp
	$(BIM2BIN) in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

7.BM5: 7.bmp
	$(BIM2BIN) in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

8.BM5: 8.bmp
	$(BIM2BIN) in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

9.BM5: 9.bmp
	$(BIM2BIN) in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

HENORMAL.BM5: henormal.bmp
	$(BIM2BIN) in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

HEPUSHED.BM5: hepushed.bmp
	$(BIM2BIN) in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

-include dependencies
