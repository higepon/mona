TARGET  = IME
SOURCES = main.cpp ImeServer.cpp
INSTDIR = $(BINDIR)/APP/
#INSTFILES  =
CLEANFILES = BASICDIC.TX5

$(TARGET).EX5: $(CLEANFILES)

SHAREDIR   = $(MONADIR)/share
include $(SHAREDIR)/configs/monapi-ex5.inc

ADDINCLUDE= -I .

BASICDIC.TX5: BASICDIC.TXT
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

install-ime:
	$(INSTALL) BASICDIC.TX5 $(MONADIR)/bin/iso

install: install-ime

-include dependencies
