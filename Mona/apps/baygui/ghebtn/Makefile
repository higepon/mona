TARGET  = GHEBTN
SOURCES = ghebtn.cpp 
INSTDIR = $(BINDIR)/root/APPS/GHEBTN.APP
INSTFILES = 0.BM2 1.BM2 2.BM2 3.BM2 4.BM2 5.BM2 6.BM2 7.BM2 8.BM2 9.BM2 HENORMAL.BM2 HEPUSHED.BM2

$(TARGET).EX5: $(INSTFILES)

MONADIR    = ../../..
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lbaygui-imp
ADDLINKDEP = $(MONADIR)/lib/libbaygui-imp.a
include $(MONADIR)/env/monapi-ex5.inc
else
ADDLINK    = -lbaygui
ADDLINKDEP = $(MONADIR)/lib/libbaygui.a
include $(MONADIR)/env/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG

0.BM2: 0.bmp
	bzip2 -c $< > $@

1.BM2: 1.bmp
	bzip2 -c $< > $@

2.BM2: 2.bmp
	bzip2 -c $< > $@

3.BM2: 3.bmp
	bzip2 -c $< > $@

4.BM2: 4.bmp
	bzip2 -c $< > $@

5.BM2: 5.bmp
	bzip2 -c $< > $@

6.BM2: 6.bmp
	bzip2 -c $< > $@

7.BM2: 7.bmp
	bzip2 -c $< > $@

8.BM2: 8.bmp
	bzip2 -c $< > $@

9.BM2: 9.bmp
	bzip2 -c $< > $@

HENORMAL.BM2: henormal.bmp
	bzip2 -c $< > $@

HEPUSHED.BM2: hepushed.bmp
	bzip2 -c $< > $@

include dependencies
