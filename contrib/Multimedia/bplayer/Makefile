ifndef $(MONADIR)
# export MONADIR=$(PWD)/../mona
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

TARGET  = BPLAYER
SOURCES = bplayer.cpp PlayFrame.cpp

INSTDIR = $(BINDIR)/APPS/BAYGUI/BPLAYER.APP
INSTFILES = PLAY.JPG\
            PLAY_F.JPG\
            STOP_F.JPG\
            BACK.JPG\
            BACK_F.JPG\
            FWD.JPG\
            FWD_F.JPG\
            P1.JPG\
            P2.JPG\
            P3.JPG\
            BAR0.JPG\
            BAR1.JPG\
            BAR2.JPG\
            BAR3.JPG\
            BAR4.JPG\
            USER.JPG\
            $(shell ls MUSIC/*.MML)
CLEANFILES =
$(TARGET).EX5: $(INSTFILES)

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lbaygui-imp -lmonalibc-imp
ADDLINKDEP = $(MONADIR)/lib/libbaygui-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lbaygui-imp -lmonalibc-imp -lmonapi-imp
ADDLINKDEP = $(MONADIR)/lib/libbaygui.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

# overwrite install
install: all
	mkdir -p $(INSTDIR)
	$(INSTALL) -p $(TARGET).EX5 $(INSTFILES) $(INSTDIR)


CXXFLAGS += -DDEBUG
INCLUDE += -I .

-include dependencies
