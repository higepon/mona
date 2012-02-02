CATEGORY = Devel
NAME     = icu4c
VERSION  = 4_4_2

ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif
include $(MONADIR)/share/configs/Makefile.inc

SOURCE1 = ${NAME}-${VERSION}-src.tgz
DATA= icudt44l_dat.s.tar.bz2
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
	$(MAKE) -f Makefile.uc clean
	$(MAKE) -f Makefile.i18n clean
distclean: clean
	@for f in ${SRCS}; do rm -f ${DSTDIR}/$${f}; done
	rm -f icudt44l_dat.s
	rm -rf icu
	rm -f icudt44l_dat.s.tar.bz2

.FETCH: ${SRCS}
	@if [ ! -f $@ ]; then touch $@; fi
DOWNLOAD = mkdir -p ${MONA_TMPDIR} ${DSTDIR} && cd ${MONA_TMPDIR} && rm -f $@ && wget
${SOURCE1}:
	@if [ ! -f ${DSTDIR}/$@ ]; then \
	  (${DOWNLOAD} ${MASTERSITE}/$@) && \
	  mv ${MONA_TMPDIR}/$@ ${DSTDIR} && \
	  wget $(MASTERSITE)/$(DATA); \
	fi

.EXTRACT: .FETCH
	tar xvzf ${DSTDIR}/${SOURCE1}
	tar xvjf ${DSTDIR}/$(DATA)
	touch $@

.PATCH: .EXTRACT Makefile.i18n platform.h Makefile.uc
	patch -p0 < patch-aa
	cp platform.h icu/source/common/unicode/
	cp icudt44l_dat.s icu/source/common/
	touch $@

.MAKE: .PATCH
	$(MAKE) -f Makefile.uc
	$(MAKE) -f Makefile.i18n
	touch $@

.INSTALL: .MAKE
	install -p -m 0644 *.DLL ${MONADIR}/bin/LIBS
	install -p $/lib*.a ${MONADIR}/lib
	touch $@
