CATEGORY = Devel
NAME     = gmp
VERSION  = 4.3.2

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

.PATCH: .EXTRACT Makefile.mona config.h dll_main.cpp
	(cd $(WORKDIR) && ./configure --enable-assert=yes --enable-cxx=no --build=i586 --host=i586-mingw32msvc && make)
	TMPDIR=$(TMP) patch -p0 < patch-aa
	cp dll_main.cpp $(WORKDIR)/
	cp config.h $(WORKDIR)/
	cp Makefile.mona $(WORKDIR)/
	cp Makefile.mona.mpn $(WORKDIR)/mpn/Makefile.mona
	(cd $(WORKDIR) && make -f Makefile.mona clean)
	touch $@

.MAKE: .PATCH
	cd ${WORKDIR} && make -f Makefile.mona
	touch $@

.INSTALL: .MAKE
	install -p -m 0644 ${WORKDIR}/*.DLL ${MONADIR}/bin/LIBS
	install -p ${WORKDIR}/lib*.a ${MONADIR}/lib
	mkdir -p ${MONADIR}/include/
	install -p ${WORKDIR}/gmp.h ${MONADIR}/include/
