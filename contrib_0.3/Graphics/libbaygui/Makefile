LIB_STATIC   = libbaygui.a
TARGET       = $(LIB_STATIC)

include $(MONADIR)/share/configs/Makefile.inc
include $(MONADIR)/share/configs/dirnames.inc
include $(MONADIR)/share/configs/monapi.inc

ifneq ($(BUILD_TARGET),ELF)
LIB_IMPORT   = libbaygui-imp.a
LIB_DYNAMIC  = BAYGUI.DLL
LIB_DYNAMIC5 = BAYGUI.DL5
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
	lang/Object.cpp \
	lang/String.cpp \
	lang/Thread.cpp \
	util/Hashtable.cpp \
	util/Vector.cpp

OBJECTS = $(SOURCES:.cpp=.o)
INCLUDE = -I$(INCDIR) -I. -I..

all : $(TARGET)

$(LIB_STATIC): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

ifneq ($(BUILD_TARGET),ELF)
$(LIB_IMPORT): $(LIB_DYNAMIC)
$(LIB_DYNAMIC): $(OBJECTS)
	$(LD) --export-all-symbols --out-implib $(LIB_IMPORT) -o $@ $(OBJECTS) -L$(MONADIR)/lib -lmonapi-imp
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
	$(INSTALL) -m 0644 $(LIB_STATIC) $(LIBDIR)/
ifneq ($(BUILD_TARGET),ELF)
	$(INSTALL) -m 0644 $(LIB_IMPORT) $(LIBDIR)/
	mkdir -p $(BINDIR)/LIBS
	$(INSTALL) $(LIB_DYNAMIC5) $(BINDIR)/LIBS/
endif
	mkdir -p $(INCDIR)/baygui
	mkdir -p $(INCDIR)/baygui/awt
	mkdir -p $(INCDIR)/baygui/awt/event
	mkdir -p $(INCDIR)/baygui/lang
	mkdir -p $(INCDIR)/baygui/util
	mkdir -p $(BINDIR)/APPS/
	$(INSTALL) baygui.h $(INCDIR)
	$(INSTALL) -m 0644 ./baygui/awt/*.h $(INCDIR)/baygui/awt
	$(INSTALL) -m 0644 ./baygui/awt/event/*.h $(INCDIR)/baygui/awt/event
	$(INSTALL) -m 0644 ./baygui/lang/*.h $(INCDIR)/baygui/lang
	$(INSTALL) -m 0644 ./baygui/util/*.h $(INCDIR)/baygui/util
	$(INSTALL) BAYGUI.MSH $(BINDIR)/APPS
	$(INSTALL) BAYGUI.INI $(BINDIR)/APPS

uninstall:
	rm -f $(LIBDIR)/$(LIB_STATIC)
ifneq ($(BUILD_TARGET),ELF)
	rm -f $(LIBDIR)/$(LIB_IMPORT)
	rm -f $(BINDIR)/LIBS/$(LIB_DYNAMIC5)
endif
	rm -f $(INCDIR)/baygui.h
	rm -f $(INCDIR)/baygui/awt/*.h
	rm -f $(INCDIR)/baygui/awt/event/*.h
	rm -f $(INCDIR)/baygui/lang/*.h
	rm -f $(INCDIR)/baygui/util/*.h
	rm -r $(INCDIR)/baygui/util
	rm -r $(INCDIR)/baygui/lang
	rm -r $(INCDIR)/baygui/awt/event
	rm -r $(INCDIR)/baygui/awt
	rm -r $(INCDIR)/baygui
	rm -f $(BINDIR)/APPS/BAYGUI.INI
	rm -f $(BINDIR)/APPS/BAYGUI.MSH

include dependencies
