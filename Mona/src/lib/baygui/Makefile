LIB_STATIC   = $(LIBDIR)/libbaygui.a
TARGET       = $(LIB_STATIC)

ifneq ($(BUILD_TARGET),ELF)
LIB_IMPORT   = $(LIBDIR)/libbaygui-imp.a
LIB_DYNAMIC  = BAYGUI.DLL
LIB_DYNAMIC2 = BAYGUI.DL5
TARGET      += $(LIB_IMPORT) $(LIB_IMPORT) $(LIB_DYNAMIC) $(LIB_DYNAMIC2)
endif

SOURCES = main.cpp Bitmap.cpp Button.cpp Control.cpp Container.cpp Event.cpp Font.cpp \
FontManager.cpp Graphics.cpp Icon.cpp Image.cpp KeyEvent.cpp Label.cpp \
ListBox.cpp MouseEvent.cpp Object.cpp Rect.cpp String.cpp TextField.cpp TimerEvent.cpp \
TwoWayItem.cpp TwoWayList.cpp Window.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi.inc

CXXFLAGS += -DDEBUG

INCLUDE = -I$(INCDIR)

all: $(TARGET)

$(LIB_STATIC): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

ifneq ($(BUILD_TARGET),ELF)
$(LIB_IMPORT): $(LIB_DYNAMIC)
$(LIB_DYNAMIC): $(OBJECTS)
	$(LD) --export-all-symbols --out-implib $(LIB_IMPORT) -o $@ $(OBJECTS) -L$(MONADIR)/lib -lmonapi-imp
	$(STRIP) $@

$(LIB_DYNAMIC2): $(LIB_DYNAMIC)
#	bzip2 -c $(LIB_DYNAMIC) > $@
	$(MONADIR)/bin/bim2bin in:$(LIB_DYNAMIC) out:$@ -osacmp -tek5 BS:0 eprm:z0
endif

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $(<:.cpp=.o) $(CXXFLAGS) $(INCLUDE) $<

clean:
	rm -f $(OBJECTS) $(TARGET)

depend:
	rm -f dependencies
	for src in $(SOURCES); \
	do \
		echo -n `echo $$src | sed "s/\(.*\)\/.*$$/\1\//"` >> dependencies; \
		$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $$src >> dependencies; \
	done

install: $(TARGET)
ifneq ($(BUILD_TARGET),ELF)
	mkdir -p $(MONADIR)/bin/root/LIBS
	$(INSTALL) $(LIB_DYNAMIC2) $(MONADIR)/bin/root/LIBS
endif

include dependencies
