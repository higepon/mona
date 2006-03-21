TARGET  = PCIINFO
SOURCES = pciinfo.cpp

INSTDIR    = $(BINDIR)/iso/APPS/$(TARGET).APP
INSTFILES  = PCIINF.BZ5
CLEANFILES = $(INSTFILES)

MONADIR    = ../../..
ADDLINK    = -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex5.inc
else
include $(MONADIR)/env/monapi-el5.inc
endif

all: $(INSTFILES)

.SUFFIXES: .TXT .BZ5
#.TXT.BZ2:
#	bzip2 -c $< > $@
.TXT.BZ5:
	$(MONADIR)/bin/bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

include dependencies
