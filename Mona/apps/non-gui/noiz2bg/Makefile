TARGET  = NOIZ2BG.ELF
SOURCES = noiz2bg.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi.inc

INCLUDE = -I$(INCDIR)
LINK    = -lmonapi -lmonalibc

all: $(TARGET)

$(TARGET): $(OBJECTS) $(LIBDIR)/libmonapi.a $(LIBDIR)/libmonalibc.a
	$(LD) $(LFLAGS) $(OBJECTS) -L$(LIBDIR) -o $@ $(LINK)
	objcopy $(OFLAGS) $@
	$(STRIP) $@
	$(BINDIR)/monaelf $@

install: all
	mkdir -p $(BINDIR)/root/APPS
	$(INSTALL) $(TARGET) $(BINDIR)/root/APPS

clean:
	rm -f $(OBJECTS) $(TARGET)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

include dependencies
