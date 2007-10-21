ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

LIB_STATIC   = libgui.a
TARGET       = $(LIB_STATIC)

include $(MONADIR)/share/configs/Makefile.inc
include $(MONADIR)/share/configs/dirnames.inc

ifneq ($(BUILD_TARGET),ELF)
LIB_IMPORT   = libgui-imp.a
LIB_DYNAMIC  = GUI.DLL
LIB_DYNAMIC5 = GUI.DL5
TARGET      += $(LIB_IMPORT) $(LIB_DYNAMIC) $(LIB_DYNAMIC5)
endif

SOURCES = \
	./stub.cpp \
	System/Console.cpp \
	System/String.cpp \
	System/Drawing/Bitmap.cpp \
	System/Drawing/Font.cpp \
	System/Drawing/Graphics.cpp \
	System/Drawing/Rectangle.cpp \
	System/Mona/Info.cpp \
	System/Mona/Forms/Application.cpp \
	System/Mona/Forms/Button.cpp \
	System/Mona/Forms/Control.cpp \
	System/Mona/Forms/ControlPaint.cpp \
	System/Mona/Forms/Cursor.cpp \
	System/Mona/Forms/Form.cpp \
	System/Mona/Forms/Label.cpp \
	System/Mona/Forms/ScrollableControl.cpp \
	System/Mona/Forms/ScrollBar.cpp \
	System/Mona/Forms/Timer.cpp \
	System/Text/Encoding.cpp

OBJECTS = $(SOURCES:.cpp=.o)
INCLUDE = -I. -I$(INCDIR) -I$(INCDIR)/stlport -I..

all : $(TARGET)

$(LIB_STATIC): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

ifneq ($(BUILD_TARGET),ELF)
$(LIB_IMPORT): $(LIB_DYNAMIC)
$(LIB_DYNAMIC): $(OBJECTS)
	$(LD) -e _dllmain --export-all-symbols --out-implib $(LIB_IMPORT) -o $@ $(OBJECTS) -L$(MONADIR)/lib -lmonapi-imp -lmonalibc-imp
	$(STRIP) $@

$(LIB_DYNAMIC5): $(LIB_DYNAMIC)
#	bzip2 -c $(LIB_DYNAMIC) > $@
	bim2bin in:$(LIB_DYNAMIC) out:$@ -osacmp -tek5 BS:0 eprm:z0
endif

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $(<:.cpp=.o) $(CXXFLAGS) $(INCLUDE) $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean:
	rm -f $(TARGET) $(OBJECTS)

distclean: clean
	rm dependencies
	touch dependencies

install: $(TARGET)
	mkdir -p $(LIBDIR)
	$(INSTALL) -p -m 0644 $(LIB_STATIC) $(LIBDIR)/
ifneq ($(BUILD_TARGET),ELF)
	$(INSTALL) -p -m 0644 $(LIB_IMPORT) $(LIBDIR)/
	mkdir -p $(BINDIR)/LIBS
	$(INSTALL) -p $(LIB_DYNAMIC5) $(BINDIR)/LIBS/
endif
	mkdir -p $(INCDIR)/gui
	mkdir -p $(INCDIR)/gui/System
	mkdir -p $(INCDIR)/gui/System/Collections
	mkdir -p $(INCDIR)/gui/System/Drawing
	mkdir -p $(INCDIR)/gui/System/Mona
	mkdir -p $(INCDIR)/gui/System/Mona/Forms
	mkdir -p $(INCDIR)/gui/System/Text
	mkdir -p $(BINDIR)/APPS/
	$(INSTALL) -p -m 0644 ./gui/System/*.h $(INCDIR)/gui/System
	$(INSTALL) -p -m 0644 ./gui/System/Collections/*.h $(INCDIR)/gui/System/Collections
	$(INSTALL) -p -m 0644 ./gui/System/Drawing/*.h $(INCDIR)/gui/System/Drawing
	$(INSTALL) -p -m 0644 ./gui/System/Mona/Forms/*.h $(INCDIR)/gui/System/Mona/Forms
	$(INSTALL) -p -m 0644 ./gui/System/Mona/*.h $(INCDIR)/gui/System/Mona
	$(INSTALL) -p -m 0644 ./gui/System/Text/*.h $(INCDIR)/gui/System/Text
	$(INSTALL) -p MONAFRMS.MSH $(BINDIR)/APPS
	$(INSTALL) -p MONAFRMS.INI $(BINDIR)/APPS
	$(INSTALL) -p MONAFUN.JPG $(BINDIR)/APPS
	$(INSTALL) -p MONAFUN.BMP $(BINDIR)/APPS

uninstall:
	rm -f $(LIBDIR)/$(LIB_STATIC)
ifneq ($(BUILD_TARGET),ELF)
	rm -f $(LIBDIR)/$(LIB_IMPORT)
	rm -f $(BINDIR)/LIBS/$(LIB_DYNAMIC5)
endif
	rm -f $(INCDIR)/gui/System/*.h
	rm -f $(INCDIR)/gui/System/Collections/*.h
	rm -f $(INCDIR)/gui/System/Drawing/*.h
	rm -f $(INCDIR)/gui/System/Mona/Forms/*.h
	rm -f $(INCDIR)/gui/System/Text/*.h
	rm -r $(INCDIR)/gui/System/Text
	rm -r $(INCDIR)/gui/System/Mona/Forms
	rm -r $(INCDIR)/gui/System/Mona
	rm -r $(INCDIR)/gui/System/Drawing
	rm -r $(INCDIR)/gui/System/Collections
	rm -r $(INCDIR)/gui/System
	rm -r $(INCDIR)/gui
	rm -f $(BINDIR)/APPS/MONAFRMS.INI
	rm -f $(BINDIR)/APPS/MONAFRMS.MSH
	rm -f $(BINDIR)/APPS/MONAFUN.JPG

-include dependencies
