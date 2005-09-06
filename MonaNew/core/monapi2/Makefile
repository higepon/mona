LIB_STATIC   = libmonapi2.a
TARGET       = $(LIB_STATIC)

include $(MONADIR)/share/configs/Makefile.inc
include $(MONADIR)/share/configs/dirnames.inc

ifneq ($(BUILD_TARGET),ELF)
LIB_IMPORT   = libmonapi2-imp.a
LIB_DYNAMIC  = MONAPI2.DLL
LIB_DYNAMIC5 = MONAPI2.DL5
TARGET      += $(LIB_IMPORT) $(LIB_DYNAMIC) $(LIB_DYNAMIC5)
endif

SOURCES = Basic/Bit.cpp \
          Basic/FileFn.cpp \
          Basic/Memory.cpp \
          Basic/Buffer.cpp \
          Basic/FileOther.cpp \
          Basic/Search.cpp \
          Basic/CollectionArray.cpp \
          Basic/Language.cpp \
          Basic/String.cpp \
          Basic/CollectionList.cpp \
          Basic/Language2.cpp \
          Basic/StringFn.cpp \
          Basic/CollectionMap.cpp \
          Basic/LanguageChar.cpp \
          Basic/StringOther.cpp \
          Basic/Debug.cpp \
          Basic/LanguageOther.cpp \
          Basic/Time.cpp \
          Basic/File.cpp \
          Basic/Math.cpp \
          MultiMedia/Bitmap.cpp \
          MultiMedia/Point.cpp \
          MultiMedia/Size.cpp \
          MultiMedia/Color.cpp \
          MultiMedia/Rect.cpp \
          OS/IniManager.cpp
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
	mkdir -p $(INCDIR)/monapi2/Basic
	mkdir -p $(INCDIR)/monapi2/MultiMedia
	mkdir -p $(INCDIR)/monapi2/OS
	$(INSTALL) -m 0644 Basic/*.h $(INCDIR)/monapi2/Basic
	$(INSTALL) -m 0644 MultiMedia/*.h $(INCDIR)/monapi2/MultiMedia
	$(INSTALL) -m 0644 OS/*.h $(INCDIR)/monapi2/OS

uninstall:
	rm -f $(LIBDIR)/$(LIB_STATIC)
ifneq ($(BUILD_TARGET),ELF)
	rm -f $(LIBDIR)/$(LIB_IMPORT)
	rm -f $(BINDIR)/LIBS/$(LIB_DYNAMIC5)
endif
	for file in *.h; \
	do \
	  rm -f $(INCDIR)/monapi2/Basic/$$file; \
	done
	for file in *.h; \
	do \
	  rm -f $(INCDIR)/monapi2/MultiMedia/$$file; \
	done
	for file in *.h; \
	do \
	  rm -f $(INCDIR)/monapi2/OS/$$file; \
	done

include dependencies
