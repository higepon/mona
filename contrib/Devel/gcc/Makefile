ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

CATEGORY = Devel
NAME     = go_0020s
VERSION  = 

SOURCE1 = ${NAME}.tar.gz
SRCS = ${SOURCE1}
MASTERSITE = http://www.monaos.org/downloads
WORKDIR = $(NAME)

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

.PATCH: .EXTRACT patch-aa
	patch -p0 < patch-aa
	touch $@

.MAKE: .PATCH
	cp rules.host.mak ${WORKDIR}/include/rules.mak
	cd ${WORKDIR}/toolstdc && make
	cp rules.target.mak ${WORKDIR}/include/rules.mak
	cd ${WORKDIR}/gcc && make
	touch $@

.INSTALL: .MAKE
	install -p -m 0644 ${WORKDIR}/gcc/*.exe ${MONADIR}/bin/APPS
