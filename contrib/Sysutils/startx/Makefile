ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

TARGET  = STARTX
SOURCES = main.cpp
INSTDIR = $(BINDIR)/APPS/STARTX.APP
INSTFILES  = LOGOC0.JPG LOGOB0.JPG LOGOM0.JPG LOGOC1.JPG LOGOB1.JPG LOGOM1.JPG MONAHERE.JPG

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

ADDLINK    = -ljpegls
ADDLINKDEP = $(MONADIR)/lib/libjpegls.a

$(TARGET).EXE: $(INSTFILES)

include dependencies
