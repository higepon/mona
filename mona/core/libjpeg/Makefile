CATEGORY = Devel
NAME     = jpeg
VERSION  = 8d

ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif
include $(MONADIR)/share/configs/Makefile.inc

SOURCE1 = jpegsrc.v8d.tar.gz
DATA=
SRCS = ${SOURCE1}
MASTERSITE = http://monaos.org/downloads
WORKDIR = ${NAME}-${VERSION}

MONA_TMPDIR = tmp
DSTDIR = .

PROCESSES = .FETCH .EXTRACT .PATCH .MAKE .INSTALL
all    : .MAKE
fetch  : .FETCH
extract: .EXTRACT
patch  : .PATCH
make   : .MAKE
install: .INSTALL
clean  :
	rm -rf tmp ${WORKDIR} ${PROCESSES}
distclean: clean
	@for f in ${SRCS}; do rm -f ${DSTDIR}/$${f}; done

.FETCH: ${SRCS}
	@if [ ! -f $@ ]; then touch $@; fi
DOWNLOAD = mkdir -p ${MONA_TMPDIR} ${DSTDIR} && cd ${MONA_TMPDIR} && rm -f $@ && wget
${SOURCE1}:
	@if [ ! -f ${DSTDIR}/$@ ]; then \
	  (${DOWNLOAD} ${MASTERSITE}/$@) && \
	  mv ${MONA_TMPDIR}/$@ ${DSTDIR}; \
	fi

.EXTRACT: .FETCH
	tar xvzf ${DSTDIR}/${SOURCE1}
	touch $@

.PATCH: .EXTRACT
	cp Makefile.mona $(WORKDIR)/Makefile
	cp jconfig.h $(WORKDIR)/
	touch $@

.MAKE: .PATCH
	$(MAKE) -C $(WORKDIR)
	touch $@

.INSTALL: .MAKE
	install -p -m 0644 $(WORKDIR)/*.DLL ${MONADIR}/bin/LIBS
	install -p $(WORKDIR)/lib*.a ${MONADIR}/lib
	mkdir -p ${MONADIR}/include/
	install -p ${WORKDIR}/jpeglib.h ${MONADIR}/include/
	install -p ${WORKDIR}/jmorecfg.h ${MONADIR}/include/
	install -p ${WORKDIR}/jconfig.h ${MONADIR}/include/
	touch $@
