TARGET  = MONES
SOURCES = mones.cpp            \
          MoEther.cpp          \
          MoArp.cpp            \
          MoIp.cpp             \
          MoTcp.cpp            \
          MoUdp.cpp            \
          MoIcmp.cpp           \
          MonesConfig.cpp      \
          MoPacUtl.cpp         \
          SocketsManager.cpp

INSTDIR = $(BINDIR)/SERVERS

ADDLINK    = -lnicdrv -lPci -lmonesoc -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

INCLUDE = -I$(INCDIR) -I. -I..

include dependencies
