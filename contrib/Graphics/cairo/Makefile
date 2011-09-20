CATEGORY = Graphics
NAME     = cairo
VERSION  = 1.10.2

SOURCE1 = ${NAME}-${VERSION}.tar.gz
SRCS = ${SOURCE1}
MASTERSITE = http://www.monaos.org/downloads
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

.PATCH: .EXTRACT config.h Makefile.mona Makefile cairo-mona-surface.cpp cairo-mona.h cairo-features.h
	cp Makefile.mona ${WORKDIR}/src/Makefile
	cp cairo-features.h ${WORKDIR}/src/
	cp cairo-mona.h ${WORKDIR}/src/
	cp cairo-mona-surface.cpp ${WORKDIR}/src/
	cp config.h ${WORKDIR}/
	touch $@

.MAKE: .PATCH
	(cd ${WORKDIR}/src && make) && touch $@

.INSTALL: .MAKE
	(cd ${WORKDIR}/src && make install) && touch $@
