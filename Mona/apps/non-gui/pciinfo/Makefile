TARGET  = PCIINFO
SOURCES = pciinfo.cpp

INSTDIR    = $(BINDIR)/root/APPS/PCIINFO.APP
INSTFILES  = PCIINF.BZ2
CLEANFILES = $(INSTFILES)

MONADIR = ../../..
include $(MONADIR)/env/monapi-el2.inc


all: $(INSTFILES)

.SUFFIXES: .TXT .BZ2
.TXT.BZ2:
	bzip2 -c $< > $@

include dependencies
