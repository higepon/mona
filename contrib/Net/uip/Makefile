CATEGORY = net
NAME     = uip
VERSION  = 0.9

SOURCE1 = ${NAME}-${VERSION}.tar.gz
SRCS = ${SOURCE1}
MASTERSITE = http://monaos.org/downloads
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
	cp $(PWD)/Mona/html/about.html ${WORKDIR}/apps/httpd/fs/
	mkdir -p ${WORKDIR}/apps/httpd/fs/img
	cp $(PWD)/Mona/html/img/logo.jpg ${WORKDIR}/apps/httpd/fs/img/
	patch -p0 < patch-aa
	touch $@

.MAKE: .PATCH
	cd ${PWD}/Mona && make
#	touch $@

.INSTALL: .MAKE
	cd ${PWD}/Mona && make install
