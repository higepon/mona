TARGET  = fat_write$(EXEEXT)
SOURCES = main.cpp VirtualFloppy.cpp fat.cpp

include $(MONADIR)/share/configs/Makefile.inc
include $(MONADIR)/share/configs/dirnames.inc

CXXFLAGS = -Wall
INCLUDE  = -idirafter .

OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)
	strip $(TARGET)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean :
	rm -f $(OBJECTS) $(TARGET)

distclean: clean
	rm dependencies
	touch dependencies

install: $(TARGET)
	mkdir -p $(PREFIX)/bin
	$(INSTALL) $(TARGET) $(PREFIX)/bin/
	mkdir -p $(INCDIR)/fat_write
	$(INSTALL) -m 0644 *.h fat.cpp $(INCDIR)/fat_write/
	mkdir -p $(SHAREDIR)/fat_write
	$(INSTALL) -m 0644 fat_template.img $(SHAREDIR)/fat_write/

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)
	rm -rf $(INCDIR)/fat_write
	rm -rf $(SHAREDIR)/fat_write

dependencies: depend
-include dependencies
