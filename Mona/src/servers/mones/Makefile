TARGET  = MONES
SOURCES = mones.cpp            \
          AbstractMonic.cpp    \
          Ne2000MoNic.cpp      \
          MoEther.cpp          \
          MoArp.cpp            \
          MonesConfig.cpp      \
          MoPacUtl.cpp

INSTDIR = $(BINDIR)/root/SERVERS

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex2.inc
else
include $(MONADIR)/env/monapi-bn2.inc
endif

include dependencies
