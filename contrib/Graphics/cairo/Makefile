LIB_STATIC   = libcairo.a
TARGET       = $(LIB_STATIC)

LIB_IMPORT   = libcairo-imp.a
LIB_DYNAMIC  = CAIRO.DLL
LIB_DYNAMIC5 = CAIRO.DL5
TARGET      += $(LIB_IMPORT) $(LIB_DYNAMIC) $(LIB_DYNAMIC5)

include $(MONADIR)/share/configs/Makefile.inc
include $(MONADIR)/share/configs/dirnames.inc

SOURCES= # dll_main.cpp

TOP_CSOURCES= cairo-analysis-surface.c cairo-arc.c cairo-array.c cairo-atomic.c cairo-base64-stream.c cairo-base85-stream.c cairo-bentley-ottmann.c cairo-bentley-ottmann-rectangular.c cairo-bentley-ottmann-rectilinear.c cairo-botor-scan-converter.c cairo-boxes.c cairo.c cairo-cache.c cairo-clip.c cairo-color.c cairo-composite-rectangles.c cairo-debug.c cairo-device.c cairo-fixed.c cairo-font-face.c cairo-font-face-twin.c cairo-font-face-twin-data.c cairo-font-options.c cairo-freelist.c cairo-freed-pool.c cairo-gstate.c cairo-hash.c cairo-hull.c cairo-image-info.c cairo-image-surface.c cairo-lzw.c cairo-matrix.c cairo-recording-surface.c cairo-misc.c cairo-mutex.c cairo-observer.c cairo-output-stream.c cairo-paginated-surface.c cairo-path-bounds.c cairo-path.c cairo-path-fill.c cairo-path-fixed.c cairo-path-in-fill.c cairo-path-stroke.c cairo-pattern.c cairo-pen.c cairo-polygon.c cairo-rectangle.c cairo-rectangular-scan-converter.c cairo-region.c cairo-rtree.c cairo-scaled-font.c cairo-slope.c cairo-spans.c cairo-spline.c cairo-stroke-style.c cairo-surface.c cairo-surface-fallback.c cairo-surface-clipper.c cairo-surface-offset.c cairo-surface-snapshot.c cairo-surface-subsurface.c cairo-surface-wrapper.c cairo-system.c cairo-tor-scan-converter.c cairo-toy-font-face.c cairo-traps.c cairo-unicode.c cairo-user-font.c cairo-version.c cairo-wideint.c cairo-cff-subset.c cairo-scaled-font-subsets.c cairo-truetype-subset.c cairo-type1-fallback.c cairo-type1-subset.c cairo-type3-glyph-surface.c cairo-pdf-operators.c 

# cairo-png.c


CSOURCES = $(TOP_CSOURCES)
CFLAGS+=-DHAVE_CONFIG_H -D_FORTIFY_SOURCE=2 -DCAIRO_NO_MUTEX=1 -Wno-attributes -Wno-enum-compare -Wno-unused-but-set-variable

OBJECTS = $(SOURCES:.cpp=.o) $(CSOURCES:.c=.o)
INCLUDE = -I. -I.. -I$(INCDIR)/ -I$(INCDIR)/monalibc -I$(INCDIR)/pixman-1

all :$(TARGET)

$(LIB_STATIC): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

$(LIB_IMPORT): $(LIB_DYNAMIC)
$(LIB_DYNAMIC): $(OBJECTS)
	$(LD) -e _dllmain --export-all-symbols --out-implib $(LIB_IMPORT) -o $@ $(OBJECTS) -L$(MONADIR)/lib -lmonapi-imp -lmonalibc-imp -lpixman-imp
	$(STRIP) $@

$(LIB_DYNAMIC5): $(LIB_DYNAMIC)
#	bzip2 -c $(LIB_DYNAMIC) > $@
	bim2bin in:$(LIB_DYNAMIC) out:$@ -osacmp -tek5 BS:0 eprm:z0

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $(<:.cpp=.o) $(CXXFLAGS) $(INCLUDE) $<

.SUFFIXES: .c .o
.c.o:
	$(CC) -c -o $(<:.c=.o) $(CFLAGS) -pedantic -std=gnu99 $(INCLUDE) $<


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
	$(INSTALL) -p -m 0644 $(LIB_IMPORT) $(LIBDIR)/
	mkdir -p $(BINDIR)/LIBS
	$(INSTALL) -p $(LIB_DYNAMIC5) $(BINDIR)/LIBS/
	mkdir -p $(INCDIR)/libxml2/libxml
	for i in `ls include/libxml/*.h`; do $(INSTALL) -p -m 0644 $$i $(INCDIR)/libxml2/libxml/; done

-include dependencies

