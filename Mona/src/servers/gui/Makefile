TARGET  = GUI.SVR
SOURCES = main.cpp MemoryInfo.cpp file.cpp image.cpp screen.cpp utils.cpp bzip2.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi.inc

INCLUDE = -I$(INCDIR) -I.
LINK    = $(LIBDIR)/monapi.o -lmonapi -lmonalibc -ljpegls

all: $(TARGET)

$(TARGET): $(OBJECTS) $(LIBMONAPI) $(LIBDIR)/libmonalibc.a $(LIBDIR)/libjpegls.a
	$(LD) $(LFLAGS) $(OBJECTS) -L$(LIBDIR) -o $@ $(LINK)
	objcopy $(OFLAGS) $@
	$(STRIP) $@
	$(BINDIR)/monaelf $@

install: all
	mkdir -p $(BINDIR)/root/SERVERS
	$(INSTALL) $(TARGET) $(BINDIR)/root/SERVERS

clean:
	rm -f $(OBJECTS) $(TARGET)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

include dependencies
