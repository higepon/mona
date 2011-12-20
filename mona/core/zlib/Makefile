CATEGORY = Devel
NAME     = zlib
VERSION  = 1.2.5

ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif
include $(MONADIR)/share/configs/Makefile.inc

SOURCE1 = ${NAME}-${VERSION}.tar.gz
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
	$(MAKE) -f Makefile.zlib clean
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
	patch -p0 < patch-aa
	touch $@

.MAKE: .PATCH
	$(MAKE) -f Makefile.zlib
	touch $@

.INSTALL: .MAKE
	install -p -m 0644 *.DLL ${MONADIR}/bin/LIBS
	install -p lib*.a ${MONADIR}/lib
	mkdir -p ${MONADIR}/include/
	install -p ${WORKDIR}/zlib.h ${MONADIR}/include/
	install -p ${WORKDIR}/zconf.h ${MONADIR}/include/
	touch $@
