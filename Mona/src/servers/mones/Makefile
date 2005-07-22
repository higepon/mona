TARGET  = MONES
SOURCES = mones.cpp            \
          AbstractMonic.cpp    \
          Ne2000MoNic.cpp      \
          MoEther.cpp          \
          MoArp.cpp            \
          MoIp.cpp             \
          MoTcp.cpp            \
          MoUdp.cpp            \
          MoIcmp.cpp           \
          MonesConfig.cpp      \
          MonesLoader.cpp      \
          MoPacUtl.cpp         


INSTDIR = $(BINDIR)/iso/SERVERS

MONADIR = ../../..
ADDLINK    = -lmonesoc -lPci -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a

ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex5.inc
else
include $(MONADIR)/env/monapi-bn5.inc
endif

include dependencies
