SRCS = \
    accum.c \
    bitmap1.c \
    bitmap2.c \
    blendeq.c \
    blendxor.c \
    copy.c \
    cursor.c \
    depth.c \
    eval.c \
    fog.c \
    font.c \
    line.c \
    logo.c \
    nurb.c \
    olympic.c \
    overlay.c \
    point.c \
    prim.c \
    quad.c \
    select.c \
    shape.c \
    sphere.c \
    star.c \
    stencil.c \
    stretch.c \
    texture.c \
    tri.c \
    wave.c

TOP = ..
include $(TOP)/Makefile.inc
INSTDIR  = $(BINDIR)/iso/APPS/MONAFRMS/MESA/SAMPLES

install: all
	mkdir -p $(INSTDIR)
	$(INSTALL) *.exe $(INSTDIR)
