TARGET  = 1LINESH.SVR
SOURCES         = main.cpp          \
                  OneLineShell.cpp  \
                  Command.cpp       \
                  DisplayWindow.cpp \
                  Charing.cpp       \
                  CommandHistory.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi.inc

INCLUDE = -I$(INCDIR) -I.
LINK    = -lmonapi -lmonalibc

all: $(TARGET)

$(TARGET): $(OBJECTS) $(LIBDIR)/libmonapi.a $(LIBDIR)/libmonalibc.a
	$(LD) $(LFLAGS) $(OBJECTS) -L$(LIBDIR) -o $@ $(LINK)
	objcopy $(OFLAGS) $@
	$(STRIP) $@
	$(BINDIR)/monaelf $@

install: all
	cp -u $(TARGET) $(BINDIR)/root/SERVERS

clean :
	rm -f $(OBJECTS) $(TARGET)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

include dependencies
