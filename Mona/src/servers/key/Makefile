TARGET  = KEYBDMNG
SOURCES = main.cpp KeyboardManager.cpp

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex5.inc
else
include $(MONADIR)/env/monapi-bn5.inc
endif

INSTDIR = $(BINDIR)/root/SERVERS

include dependencies
