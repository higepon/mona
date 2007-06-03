CATEGORY = text
NAME     = onig
VERSION  = 5.7.0

SOURCE1 = ${NAME}-${VERSION}.tar.gz
SRCS = ${SOURCE1}
MASTERSITE = http://www.monaos.org/downloads
WORKDIR = ${NAME}-${VERSION}

TMPDIR = tmp
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
DOWNLOAD = mkdir -p ${TMPDIR} ${DSTDIR} && cd ${TMPDIR} && rm -f $@ && wget
${SOURCE1}:
	@if [ ! -f ${DSTDIR}/$@ ]; then \
	  (${DOWNLOAD} ${MASTERSITE}/$@) && \
	  mv ${TMPDIR}/$@ ${DSTDIR}; \
	fi

.EXTRACT: .FETCH
	tar xvzf ${DSTDIR}/${SOURCE1}
	touch $@

.PATCH: .EXTRACT
	cp config.h ${WORKDIR}/
	cp dllmain.cpp ${WORKDIR}/
	cp Makefile.mona ${WORKDIR}/Makefile
	touch $@

.MAKE: .PATCH
	cd ${WORKDIR} && make
	touch $@

.INSTALL: .MAKE
	cd ${WORKDIR} && make install
