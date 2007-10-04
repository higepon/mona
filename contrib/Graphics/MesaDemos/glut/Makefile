LIB_STATIC   = libglut.a
TARGET       = $(LIB_STATIC)

ifneq ($(BUILD_TARGET),ELF)
LIB_IMPORT   = libglut-imp.a
LIB_DYNAMIC  = GLUT.DLL
TARGET      += $(LIB_IMPORT) $(LIB_IMPORT) $(LIB_DYNAMIC)
endif

SOURCES = glut_MonaForms.cpp
CSOURCES = \
	glut_8x13.c \
	glut_9x15.c \
	glut_bitmap.c \
	glut_bwidth.c \
	glut_hel10.c \
	glut_hel12.c \
	glut_hel18.c \
	glut_tr10.c \
	glut_tr24.c \
	glut_mroman.c \
	glut_roman.c \
	glut_stroke.c \
	glut_swidth.c \
	glut_shapes.c \
	glut_teapot.c \
	glut_util.c \
	win32_util.c

include $(MONADIR)/env/monapi.inc

CFLAGS += -nostdinc -DGLUT_IMPORT_LIB
CXXFLAGS += -nostdinc -DGLUT_IMPORT_LIB -DDEBUG

INCLUDE = -I$(INCDIR) -I../include -I../../Mesa-6.2.1/include -I../../tmpmonalibc
LINK    = -L$(MONADIR)/lib -L../../Mesa-6.2.1/lib -L../../tmpmonalibc -lGLU-imp -lOSMesa-imp -ltmpmonalibc-imp -lgui-imp -lmonalibc-imp -lmonapi-imp

all: $(TARGET)

$(LIB_STATIC): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

ifneq ($(BUILD_TARGET),ELF)
$(LIB_IMPORT): $(LIB_DYNAMIC)
$(LIB_DYNAMIC): $(OBJECTS)
	$(LD) -s --export-all-symbols --out-implib $(LIB_IMPORT) -o $@ $(OBJECTS) $(LINK)
endif

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $(<:.cpp=.o) $(CXXFLAGS) $(INCLUDE) $<

.SUFFIXES: .c .o
.c.o:
	$(CC) -c -o $(<:.c=.o) $(CFLAGS) $(INCLUDE) $<

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
ifneq ($(BUILD_TARGET),ELF)
	$(INSTALL) -m 0644 $(LIB_DYNAMIC) $(MONADIR)/bin/iso/LIBS
endif
