TARGET  = IMESVR
SOURCES = main.cpp ImeServer.cpp
INSTDIR = $(BINDIR)/root/SERVERS
#INSTFILES  = 
#CLEANFILES = BASICDIC.TX5

$(TARGET).EX5: $(CLEANFILES)

MONADIR    = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex5.inc
else
include $(MONADIR)/env/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG

BASICDIC.TX5: BASICDIC.TXT
	$(MONADIR)/bin/bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

#install-ime:
#	$(INSTALL) BASICDIC.TX5 $(MONADIR)/bin/root

#install: install-ime

include dependencies
