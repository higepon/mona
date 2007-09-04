ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

TARGET  = AUDIO
SOURCES = test.cpp

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
TREMOR_OBJECTS = $(TREMOR_SOURCES:.c=.o)

INSTDIR = $(BINDIR)/APPS/
INSTFILES = A.OGG
CLEANFILES = $(TREMOR_OBJECTS)
$(TARGET).EX5: $(INSTFILES)

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lmonalibc-imp 
ADDLINKDEP = 
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lmonalibc-imp -lmonapi-imp 
ADDLINKDEP = 
include $(SHAREDIR)/configs/monapi-el5.inc
endif

ADDLINK += -lpci-imp

# overwrite install
install: all
	mkdir -p $(INSTDIR)
	$(INSTALL) -p $(TARGET).EX5 $(INSTFILES) $(INSTDIR)

# overwrite targe
$(TARGET).EXE:  $(ADDTARGETDEP) $(LINKDEP) $(OBJECTS) $(LINKDEP) $(TREMOR_OBJECTS)
	$(LD) --Map $(TARGET).map $(LFLAGS) -o $@ $(LIBDIR)/monapi_crt.o $(OBJECTS) _divdi3.o $(TREMOR_OBJECTS) -L$(LIBDIR) $(LINK)
	$(STRIP) $@

CXXFLAGS += -fpermissive -DDEBUG -nostdinc -fno-builtin
INCLUDE += -I . -I$(MONADIR)/include/monalibc -nostdinc
CFLAGS = -I $(TREMOR_DIR) -nostdinc -fno-builtin #-I libogg-1.1.3/include -I libvorbis-1.0/include

-include dependencies
