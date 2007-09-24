ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../../mona; pwd)
endif

TARGET  = HAPPY
SOURCES = happy.cpp CUIPlayer.cpp GUIPlayer.cpp

TREMOR_DIR = Tremor-r13614
TREMOR_SOURCES = \
$(TREMOR_DIR)/bitwise.c         \
$(TREMOR_DIR)/block.c           \
$(TREMOR_DIR)/codebook.c        \
$(TREMOR_DIR)/floor0.c          \
$(TREMOR_DIR)/floor1.c          \
$(TREMOR_DIR)/framing.c         \
$(TREMOR_DIR)/info.c            \
$(TREMOR_DIR)/mapping0.c        \
$(TREMOR_DIR)/mdct.c            \
$(TREMOR_DIR)/registry.c        \
$(TREMOR_DIR)/res012.c          \
$(TREMOR_DIR)/sharedbook.c      \
$(TREMOR_DIR)/synthesis.c       \
$(TREMOR_DIR)/vorbisfile.c      \
$(TREMOR_DIR)/window.c
TREMOR_OBJECTS = $(TREMOR_SOURCES:.c=.o) _divdi3.o

INSTDIR = $(BINDIR)/MUSIC
INSTFILES = A.OGG #C.OGG D.OGG
CLEANFILES = $(TREMOR_SOURCES:.c=.o)
$(TARGET).EX5: $(INSTFILES)

SHAREDIR   = $(MONADIR)/share
ADDLINK    = $(TREMOR_OBJECTS) -lmonalibc-imp -laudioapi-imp -lbaygui-imp
ADDLINKDEP = $(TREMOR_OBJECTS)
include $(SHAREDIR)/configs/monapi-ex5.inc

CXXFLAGS += -fpermissive -nostdinc -fno-builtin -D_LOW_ACCURACY_
INCLUDE += -I . -I$(MONADIR)/include/monalibc
CFLAGS = -I $(TREMOR_DIR) -nostdinc -fno-builtin  -D_LOW_ACCURACY_

-include dependencies
