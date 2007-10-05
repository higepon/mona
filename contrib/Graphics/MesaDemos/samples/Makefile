SRCS = \
    accum.c \
    bitmap1.c \
    bitmap2.c \
    blendeq.c \
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

#    blendxor.c \

TOP = ..
include $(TOP)/Makefile.inc
INSTDIR  = $(BINDIR)/APPS/MONAFRMS/MESA/SAMPLES

install: all
	mkdir -p $(INSTDIR)
	$(INSTALL) *.EXE $(INSTDIR)
