TARGET  = MOUSE
SOURCES = main.cpp
INSTDIR = $(BINDIR)/root/SERVERS

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex5.inc
else
include $(MONADIR)/env/monapi-bn5.inc
endif

include dependencies
