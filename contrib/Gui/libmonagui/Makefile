ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

LIB_STATIC   = libamonagui.a
TARGET       = $(LIB_STATIC)

include $(MONADIR)/share/configs/Makefile.inc
include $(MONADIR)/share/configs/dirnames.inc
include $(MONADIR)/share/configs/monapi.inc

ifneq ($(BUILD_TARGET),ELF)

## N.B.
##  The name libamonagui-imp should be libmonagui-imp.
##  But W3M always crashes with the name.
##  The name of -imp library should be come more forward then libcrypt.
##  So we use liba prefix which forces the order.
LIB_IMPORT   = libamonagui-imp.a
LIB_DYNAMIC  = MONAGUI.DLL
LIB_DYNAMIC5 = MONAGUI.DL5
TARGET      += $(LIB_IMPORT) $(LIB_DYNAMIC) $(LIB_DYNAMIC5)
endif

SOURCES = \
	main.cpp \
	awt/Button.cpp \
	awt/Checkbox.cpp \
	awt/CheckboxGroup.cpp \
	awt/Component.cpp \
	awt/Container.cpp \
	awt/FontMetrics.cpp \
	awt/Frame.cpp \
	awt/Graphics.cpp \
	awt/Image.cpp \
	awt/Label.cpp \
	awt/ListBox.cpp \
	awt/Scrollbar.cpp \
	awt/TextField.cpp \
	awt/Window.cpp \
	awt/ImeManager.cpp \
	lang/Object.cpp \
	lang/String.cpp \
	lang/Thread.cpp \
	util/Hashtable.cpp \
	util/Vector.cpp

OBJECTS = $(SOURCES:.cpp=.o)
INCLUDE =  -I. -I.. -I$(INCDIR) -I ../ime_server -I$(INCDIR)/STLport-5.2.1/stlport -I$(INCDIR)/monalibc -I$(INCDIR)/c++

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
	mkdir -p $(INCDIR)/monagui
	mkdir -p $(INCDIR)/monagui/awt
	mkdir -p $(INCDIR)/monagui/awt/event
	mkdir -p $(INCDIR)/monagui/lang
	mkdir -p $(INCDIR)/monagui/util
	mkdir -p $(BINDIR)/APPS/
	mkdir -p $(BINDIR)/APPS/MONAGUI
	$(INSTALL) -p mplus_j12r_u.bdf $(BINDIR)/APPS/MONAGUI
	$(INSTALL) -p mplus_f12r.bdf $(BINDIR)/APPS/MONAGUI
	$(INSTALL) -p monagui.h $(INCDIR)
	$(INSTALL) -p -m 0644 ./monagui/awt/*.h $(INCDIR)/monagui/awt
	$(INSTALL) -p -m 0644 ./monagui/awt/event/*.h $(INCDIR)/monagui/awt/event
	$(INSTALL) -p -m 0644 ./monagui/lang/*.h $(INCDIR)/monagui/lang
	$(INSTALL) -p -m 0644 ./monagui/util/*.h $(INCDIR)/monagui/util
	$(INSTALL) -p MONAGUI.MSH $(BINDIR)/APPS
	$(INSTALL) -p MONAGUI.INI $(BINDIR)/APPS
	$(INSTALL) -p MONAWALL.BM5 $(BINDIR)/APPS

uninstall:
	rm -f $(LIBDIR)/$(LIB_STATIC)
ifneq ($(BUILD_TARGET),ELF)
	rm -f $(LIBDIR)/$(LIB_IMPORT)
	rm -f $(BINDIR)/LIBS/$(LIB_DYNAMIC5)
endif
	rm -f $(INCDIR)/monagui.h
	rm -f $(INCDIR)/monagui/awt/*.h
	rm -f $(INCDIR)/monagui/awt/event/*.h
	rm -f $(INCDIR)/monagui/lang/*.h
	rm -f $(INCDIR)/monagui/util/*.h
	rm -r $(INCDIR)/monagui/util
	rm -r $(INCDIR)/monagui/lang
	rm -r $(INCDIR)/monagui/awt/event
	rm -r $(INCDIR)/monagui/awt
	rm -r $(INCDIR)/monagui
	rm -f $(BINDIR)/APPS/MONAGUI.INI
	rm -f $(BINDIR)/APPS/MONAGUI.MSH
	rm -f $(BINDIR)/APPS/MONAWALL.BM5

-include dependencies
