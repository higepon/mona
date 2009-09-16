ifndef $(MONADIR)
# export MONADIR=$(PWD)/../mona
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

TARGET  = AUDIO
# server / client
SOURCES = main.cpp es1370.cpp audio_driver.cpp audio_server.cpp circular_buffer.cpp channel.cpp
#SOURCES = hoge.cpp es1370.cpp audio_driver.cpp audio_server.cpp circular_buffer.cpp channel.cpp

#TREMOR_DIR = Tremor-r13614
#TREMOR_SOURCES = \
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
#TREMOR_OBJECTS = $(TREMOR_SOURCES:.c=.o)


# one binary
# SOURCES = test.cpp es1370.cpp audio_driver.cpp

INSTDIR = $(BINDIR)/APPS/
INSTFILES = test.raw happy/A.OGG
CLEANFILES = 
$(TARGET).EX5: $(INSTFILES)

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lmonalibc-imp  --enable-auto-import 
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
	$(INSTALL) -p ./servers/audio.h $(INCDIR)/servers
	(cd api && make && make install)
	(cd client && make && make install)
	(cd happy && make && make install)

# overwrite targe
#$(TARGET).EXE:  $(ADDTARGETDEP) $(LINKDEP) $(OBJECTS) $(LINKDEP) $(TREMOR_OBJECTS)
#	$(LD) --Map $(TARGET).map $(LFLAGS) -o $@ $(LIBDIR)/monapi_crt.o $(OBJECTS) $(TREMOR_OBJECTS) -L$(LIBDIR) $(LINK) -L /usr/lib/gcc/i586-mingw32msvc/3.4.5/ -lgcc
#	$(STRIP) $@



CXXFLAGS += -fpermissive #-DDEBUG
INCLUDE += -I . -I$(MONADIR)/include/monalibc
CFLAGS = #-I $(TREMOR_DIR) #-I libogg-1.1.3/include -I libvorbis-1.0/include
CXXFLAGS += #-I $(TREMOR_DIR) #-I libogg-1.1.3/include -I libvorbis-1.0/include

-include dependencies
