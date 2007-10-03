ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

TARGET    = AUDIO
SOURCES   = main.cpp es1370.cpp audio_driver.cpp audio_server.cpp circular_buffer.cpp channel.cpp
INSTDIR   = $(BINDIR)/APPS/
INSTFILES = test.raw

$(TARGET).EX5: $(INSTFILES)

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lmonalibc-imp
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lmonalibc-imp -lmonapi-imp
include $(SHAREDIR)/configs/monapi-el5.inc
endif

ADDLINK += -lpci-imp

# overwrite install
install: all
	mkdir -p $(INSTDIR)
	$(INSTALL) -p $(TARGET).EX5 $(INSTFILES) $(INSTDIR)
	$(INSTALL) -p ./servers/audio.h $(INCDIR)/servers
	(cd api && make && make install)
	(cd client && make && make install)
	(cd happy && make && make install)

CXXFLAGS += -fpermissive #-DDEBUG
INCLUDE += -I . -I$(MONADIR)/include/monalibc

-include dependencies
