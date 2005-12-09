CRT_OBJECT = crt.o
LIB_STATIC = libtmpmonalibc.a
TARGET     = $(CRT_OBJECT) $(LIB_STATIC)

ifneq ($(BUILD_TARGET),ELF)
LIB_IMPORT   = libtmpmonalibc-imp.a
LIB_DYNAMIC  = TMPMLIBC.DLL
TARGET      += $(LIB_IMPORT) $(LIB_IMPORT) $(LIB_DYNAMIC)
endif

#include $(MONADIR)/env/Makefile.inc
include $(MONADIR)/share/configs/Makefile.inc

CSOURCES = math.c stdlib.c stdio.c stdarg.c string.c setjmp.c time.c
OBJECTS  = $(CSOURCES:.c=.o)
INCLUDE  = -I./ -I$(MONADIR)/include
LINK     = -L$(MONADIR)/lib -L../Mesa-6.2.1/lib -lmonalibc-imp -lmonapi-imp

.SUFFIXES: .c .o
.c.o:
	$(CC) -c -o $@ $(CFLAGS) $(INCLUDE) $<

all: $(TARGET)

$(LIB_STATIC): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

ifneq ($(BUILD_TARGET),ELF)
$(LIB_IMPORT): $(LIB_DYNAMIC)
$(LIB_DYNAMIC): $(OBJECTS)
	$(LD) --export-all-symbols --out-implib $(LIB_IMPORT) -o $@ $(OBJECTS) $(LINK)
	$(STRIP) $@
endif

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
ifneq ($(BUILD_TARGET),ELF)
#	$(INSTALL) -m 0644 $(LIB_DYNAMIC) $(MONADIR)/bin/iso/LIBS
	$(INSTALL) -m 0644 $(LIB_DYNAMIC) $(MONADIR)/bin/LIBS
endif
