ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

TARGET  = GUI
SOURCES = main.cpp image.cpp screen.cpp utils.cpp window.cpp effect.cpp Rectangle.cpp Overlap.cpp
INSTDIR = $(BINDIR)/SERVERS

SHAREDIR   = $(MONADIR)/share

IMPSFX = -imp
include $(SHAREDIR)/configs/monapi.inc

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $@ $(CXXFLAGS) $(INCLUDE) -I. $<

all: $(TARGET).EX5

$(TARGET).EX5: $(TARGET).EXE
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

$(TARGET).EXE: $(OBJECTS) $(LINKDEP)
	$(LD) --Map $(TARGET).map $(LFLAGS) -o $@ $(LIBDIR)/monapi_crt.o $(OBJECTS) -L$(LIBDIR) $(LINK) -lmonalibc-imp -ljpegls
	$(STRIP) $@

install: all
	mkdir -p $(INSTDIR)
	mkdir -p $(INCDIR)/servers
	mkdir -p $(BINDIR)/APPS
	$(INSTALL) -p $(TARGET).EX5 $(INSTFILES) $(INSTDIR)
	$(INSTALL) -p ./servers/gui.h $(INCDIR)/servers
	$(INSTALL) -p MONA12.MF5 $(BINDIR)/APPS

clean:
	rm -f $(OBJECTS) $(TARGET).EXE $(TARGET).EX5 $(CLEANFILES) $(TARGET).map
	rm -f $(INSTDIR)/$(TARGET).EX5 $(INCDIR)/servers/gui.h
	rm -f $(BINDIR)/APPS/MONA12.MF5

depend:
	rm -f dependencies
	$(CXX) -MM $(INCLUDE) -I. $(CXXFLAGS) $(SOURCES) > dependencies

distclean: clean
	rm dependencies
	touch dependencies

include dependencies
