TARGET  = PCIINFO
SOURCES = pciinfo.cpp

INSTDIR    = $(BINDIR)/root/APPS/PCIINFO.APP
INSTFILES  = PCIINF.BZ2
CLEANFILES = $(INSTFILES)

MONADIR    = ../../..
ADDLINK    = -lmonalibc -ljpegls
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a $(MONADIR)/lib/libjpegls.a
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex2.inc
else
include $(MONADIR)/env/monapi-el2.inc
endif

all: $(INSTFILES)

.SUFFIXES: .TXT .BZ2
.TXT.BZ2:
	bzip2 -c $< > $@

include dependencies
