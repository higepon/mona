TARGET  = GUI.SVR
SOURCES = main.cpp image.cpp screen.cpp utils.cpp window.cpp effect.cpp Rectangle.cpp Overlap.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi.inc

INSTDIR = $(BINDIR)/root/SERVERS
LINK    = $(LIBDIR)/monapi.o -lmonapi -lmonalibc -ljpegls

all: $(TARGET)

$(TARGET): $(OBJECTS) $(LIBMONAPI) $(LIBDIR)/libmonalibc.a $(LIBDIR)/libjpegls.a
	$(LD) $(LFLAGS) $(OBJECTS) -L$(LIBDIR) -o $@ $(LINK)
	objcopy $(OFLAGS) $@
	$(STRIP) $@
	$(BINDIR)/monaelf $@

install: all
	mkdir -p $(INSTDIR)
	$(INSTALL) $(TARGET) $(INSTFILES) $(INSTDIR)
	$(INSTALL) STARTGUI.MSH $(MONADIR)/bin/root/APPS

clean:
	rm -f $(OBJECTS) $(TARGET)

depend:
	rm -f dependencies
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) >> dependencies

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $@ $(CXXFLAGS) $(INCLUDE) $<

include dependencies
