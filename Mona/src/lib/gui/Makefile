LIB_STATIC   = $(LIBDIR)/libgui.a
TARGET       = $(LIB_STATIC)

ifneq ($(BUILD_TARGET),ELF)
LIB_IMPORT   = $(LIBDIR)/libgui-imp.a
LIB_DYNAMIC  = GUI.DLL
LIB_DYNAMIC2 = GUI.DL2
TARGET      += $(LIB_IMPORT) $(LIB_IMPORT) $(LIB_DYNAMIC) $(LIB_DYNAMIC2)
endif

SOURCES = \
	./stub.cpp \
	System/Console.cpp \
	System/String.cpp \
	System/Drawing/Bitmap.cpp \
	System/Drawing/Font.cpp \
	System/Drawing/Graphics.cpp \
	System/Drawing/Rectangle.cpp \
	System/Mona/Forms/Application.cpp \
	System/Mona/Forms/Button.cpp \
	System/Mona/Forms/Control.cpp \
	System/Mona/Forms/ControlPaint.cpp \
	System/Mona/Forms/Cursor.cpp \
	System/Mona/Forms/Form.cpp \
	System/Mona/Forms/Label.cpp \
	System/Mona/Forms/Timer.cpp \
	System/Text/Encoding.cpp

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
	bzip2 -c $(LIB_DYNAMIC) > $@
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
