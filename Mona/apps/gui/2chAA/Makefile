TARGET  = 2CHAA.ELF
SOURCES = Form1.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi.inc

INCLUDE = -I$(INCDIR)
LINK    = -lgui -lmonapi -lmonalibc

CXXFLAGS += -DMONA -DDEBUG

all: $(TARGET)

$(TARGET): $(OBJECTS) $(LIBDIR)/libgui.a $(LIBDIR)/libmonapi.a $(LIBDIR)/libmonalibc.a
	$(LD) $(LFLAGS) $(OBJECTS) -L$(LIBDIR) -o $@ $(LINK)
	objcopy $(OFLAGS) $@
	$(STRIP) $@

install: all
	cp -u $(TARGET) $(BINDIR)/root/APPS

clean :
	rm -f $(OBJECTS) $(TARGET)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

include dependencies
