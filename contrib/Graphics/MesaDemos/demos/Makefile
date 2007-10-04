SRCS= \
	bounce.c \
	clearspd.c \
	cubemap.c \
	drawpix.c \
	fire.c \
	gamma.c \
	gears.c \
	geartrain.c \
	glinfo.c \
	gloss.c \
	gltestperf.c \
	glutfx.c \
	isosurf.c \
	ipers.c \
	lodbias.c \
	morph3d.c \
	multiarb.c \
	occlude.c \
	paltex.c \
	pixeltex.c \
	pointblast.c \
	ray.c \
	readpix.c \
	reflect.c \
	renormal.c \
	spectex.c \
	stex3d.c \
	teapot.c \
	terrain.c \
	tessdemo.c \
	texcyl.c \
	texdown.c \
	texenv.c \
	texobj.c \
	trispd.c \
	tunnel.c \
	tunnel2.c \
	winpos.c
#	osdemo.c

TOP = ..
include $(TOP)/Makefile.inc
INSTDIR  = $(BINDIR)/iso/APPS/MONAFRMS/MESA/DEMOS

install: all
	mkdir -p $(INSTDIR)
	$(INSTALL) *.exe $(INSTDIR)
	mkdir -p $(MONADIR)/bin/iso/MESA
	$(INSTALL) -m 0644 *.dat $(MONADIR)/bin/iso/MESA
