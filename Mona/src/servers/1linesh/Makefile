TARGET = 1LINESH
SOURCES         = main.cpp          \
                  OneLineShell.cpp  \
                  Command.cpp       \
                  DisplayWindow.cpp \
                  CommandHistory.cpp
INSTDIR = $(BINDIR)/root/SERVERS

MONADIR    = ../../..
ADDLINK    = -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex5.inc
else
include $(MONADIR)/env/monapi-bn5.inc
endif

include dependencies
