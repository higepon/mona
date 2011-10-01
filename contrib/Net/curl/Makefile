CATEGORY = Net
NAME     = curl
VERSION  = 7.22.0

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

.PATCH: .EXTRACT curl_config.h Makefile.mona Makefile
	cp Makefile.mona ${WORKDIR}/lib/Makefile
	cp curl_config.h ${WORKDIR}/lib/
	touch $@

.MAKE: .PATCH
	(cd ${WORKDIR}/lib && make) && touch $@

.INSTALL: .MAKE
	(cd ${WORKDIR}/lib && make install) && touch $@
