TARGET  = JPEGDEMO.ELF
SOURCES = jpegdemo.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi.inc

INCLUDE = -I$(INCDIR) -I.
LINK    = -lmonapi -lmonalibc -ljpegls

all: $(TARGET)

$(TARGET): $(OBJECTS) $(LIBDIR)/libmonapi.a $(LIBDIR)/libmonalibc.a $(LIBDIR)/libjpegls.a
	$(LD) $(LFLAGS) $(OBJECTS) -L$(LIBDIR) -o $@ $(LINK)
	objcopy $(OFLAGS) $@
	$(STRIP) $@
	$(BINDIR)/monaelf $@

install: all
	cp -u $(TARGET) $(BINDIR)/root/APPS

clean :
	rm -f $(OBJECTS) $(TARGET)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

include dependencies
