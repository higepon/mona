ifndef $(MONADIR)
# export MONADIR=$(PWD)/../mona
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

TARGET  = AUDIO
SOURCES = main.cpp Audio.cpp drivers/es1370.cpp

INSTDIR = $(BINDIR)/APPS/
INSTFILES =
CLEANFILES =
$(TARGET).EX5: $(INSTFILES)

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lmonalibc-imp
ADDLINKDEP = 
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lmonalibc-imp -lmonapi-imp
ADDLINKDEP = 
include $(SHAREDIR)/configs/monapi-el5.inc
endif

# overwrite install
install: all
	mkdir -p $(INSTDIR)
	$(INSTALL) -p $(TARGET).EX5 $(INSTFILES) $(INSTDIR)


CXXFLAGS += -DDEBUG -I$(INCDIR)/stlport
INCLUDE += -I .

-include dependencies