# Makefile for Scheme Library
# Author: Aubrey Jaffer
#
# This code is in the public domain.

SHELL = /bin/sh
intro:
	@echo
	@echo "Welcome to SLIB.  Read \"README\" and \"slib.info\" (or"
	@echo "\"slib.texi\") to learn how to install and use SLIB."
	@echo
	-make slib.info

srcdir.mk: .. Makefile
	echo "srcdir = `pwd`/" > srcdir.mk
#srcdir=$(HOME)/slib/
include srcdir.mk

VERSION = 3a4
RELEASE = 1

rpm_prefix=$(HOME)/rpmbuild/
prefix = /usr/local/
exec_prefix = $(prefix)
# directory where `make install' will put executable.
bindir = $(exec_prefix)bin/
libdir = $(exec_prefix)lib/
libslibdir = $(libdir)slib/
# directory where `make install' will put manual page.
mandir = $(prefix)man/
man1dir = $(mandir)man1/
infodir = $(prefix)info/

PREVDOCS = prevdocs/

htmldir=../public_html/

RUNNABLE = scheme48
S48INIT = scheme48.init
S48LIB = $(libdir)$(RUNNABLE)/
S48SLIB = $(S48LIB)slib/
VM = scheme48vm
IMAGE48 = slib.image
INSTALL_DATA = install -c

$(S48LIB)slibcat:
	touch $(S48LIB)slibcat

catalogs:
	-if type scm; then scm -c "(require 'new-catalog)"; fi
	-if type guile; then guile -l guile.init -c\
	  "(use-modules (ice-9 slib)) (require 'new-catalog)"; fi
	-if type umb-scheme; then SCHEME_INIT=umbscheme.init;export SCHEME_INIT;\
	  echo "(require 'new-catalog)" | umb-scheme; fi
	-if type mzscheme; then SCHEME_LIBRARY_PATH=`pwd`/;export SCHEME_LIBRARY_PATH;\
	  mzscheme -g -f DrScheme.init -e "(require 'new-catalog)" </dev/null; fi
	-if type scheme48; then make install48; fi

MKNMDB = (require 'color-database) (make-slib-color-name-db) (slib:exit)
clrnamdb: clrnamdb.scm
clrnamdb.scm:  mkclrnam.scm color.scm resenecolours.txt saturate.txt nbs-iscc.txt
	if type scm; then scm -e"$(MKNMDB)";\
	elif type guile; then guile -l guile.init -c\
	 "(use-modules (ice-9 slib)) $(MKNMDB)";\
	elif type slib48; then echo -e "$(MKNMDB)\n,exit" | slib48 -h 3000000;\
	elif type umb-scheme; then SCHEME_INIT=`pwd`/umbscheme.init;export SCHEME_INIT;\
	 echo "$(MKNMDB)" | umb-scheme;\
	elif type mzscheme; then SCHEME_LIBRARY_PATH=`pwd`/;export SCHEME_LIBRARY_PATH;\
	 echo "$(MKNMDB)" | mzscheme -f DrScheme.init;\
	fi

$(S48SLIB)strport.scm:
	test -d $(S48SLIB) || mkdir $(S48SLIB)
	echo ";;; strport.scm  -*- scheme -*-">$(S48SLIB)strport.scm
	echo ";@">>$(S48SLIB)strport.scm
	echo "(define (call-with-output-string proc)">>$(S48SLIB)strport.scm
	echo "  (let ((port (make-string-output-port)))">>$(S48SLIB)strport.scm
	echo "    (proc port)">>$(S48SLIB)strport.scm
	echo "    (string-output-port-output port)))">>$(S48SLIB)strport.scm
	echo "(define (call-with-input-string string proc)">>$(S48SLIB)strport.scm
	echo "  (proc (make-string-input-port string)))">>$(S48SLIB)strport.scm

$(S48SLIB)record.scm:
	test -d $(S48SLIB) || mkdir $(S48SLIB)
	echo ";;; record.scm  -*- scheme -*-">$(S48SLIB)record.scm
	echo ";; This code is in the public domain">>$(S48SLIB)record.scm
	echo ";@">>$(S48SLIB)record.scm
	echo "(define make-record-type   make-record-type)">>$(S48SLIB)record.scm
	echo "(define record-constructor">>$(S48SLIB)record.scm
	echo "  (let ((constructor record-constructor))">>$(S48SLIB)record.scm
	echo "    (lambda (rt . fields)">>$(S48SLIB)record.scm
	echo "      (constructor rt (if (pair? fields)">>$(S48SLIB)record.scm
	echo "                          (car fields)">>$(S48SLIB)record.scm
	echo "                          (record-type-field-names rt))))))">>$(S48SLIB)record.scm
	echo "(define record-predicate   record-predicate)">>$(S48SLIB)record.scm
	echo "(define record-accessor    record-accessor)">>$(S48SLIB)record.scm
	echo "(define record-modifier    record-modifier)">>$(S48SLIB)record.scm

slib48:	$(IMAGE48)
$(IMAGE48):	Makefile $(S48INIT)
	S48_VERSION="`echo ,exit | $(RUNNABLE) | sed -n 's/Welcome to Scheme 48 //;s/ ([^)]*)[.]//;p;q'`";export S48_VERSION;\
	S48_VICINITY="$(S48LIB)";export S48_VICINITY;\
	SCHEME_LIBRARY_PATH="`pwd`/";export SCHEME_LIBRARY_PATH;\
	$(RUNNABLE) < $(S48INIT)
install48:	$(IMAGE48) $(S48SLIB)strport.scm $(S48SLIB)record.scm
	$(INSTALL_DATA) $(IMAGE48) $(S48LIB)
	(echo '#! /bin/sh';\
	 echo exec $(RUNNABLE) -i '$(S48LIB)$(IMAGE48)' \"\$$\@\") \
	  > $(bindir)slib48
	chmod +x $(bindir)slib48

ffiles = format.scm printf.scm genwrite.scm pp.scm \
	ppfile.scm strcase.scm debug.scm trace.scm \
	strport.scm scanf.scm qp.scm break.scm stdio.scm \
	strsrch.scm prec.scm schmooz.scm defmacex.scm mbe.scm
lfiles = sort.scm comlist.scm logical.scm
revfiles = sc4opt.scm sc4sc3.scm sc2.scm mularg.scm mulapply.scm \
	trnscrpt.scm withfile.scm dynwind.scm promise.scm \
	values.scm eval.scm null.scm
afiles = charplot.scm root.scm cring.scm selfset.scm limit.scm \
	 timecore.scm psxtime.scm cltime.scm timezone.scm tzfile.scm \
	 math-real.scm
bfiles = fluidlet.scm object.scm recobj.scm yasyn.scm collect.scm collectx.scm
scfiles = r4rsyn.scm scmacro.scm synclo.scm synrul.scm synchk.scm \
	repl.scm macwork.scm mwexpand.scm mwdenote.scm mwsynrul.scm
scafiles = scainit.scm scaglob.scm scamacr.scm scaoutp.scm scaexpp.scm \
	structure.scm
srfiles = srfi-2.scm srfi-8.scm srfi-9.scm srfi-23.scm srfi-61.scm
efiles = record.scm dynamic.scm process.scm hash.scm \
	wttree.scm wttest.scm sierpinski.scm soundex.scm simetrix.scm
rfiles = rdms.scm alistab.scm paramlst.scm \
	batch.scm crc.scm dbrowse.scm getopt.scm dbinterp.scm \
	dbcom.scm dbsyn.scm
gfiles = colorspc.scm cie1931.xyz cie1964.xyz resenecolours.txt saturate.txt \
	nbs-iscc.txt ciesid65.dat ciesia.dat

txiscms =grapheps.scm glob.scm getparam.scm \
	vet.scm top-refs.scm hashtab.scm chap.scm comparse.scm\
	alist.scm ratize.scm modular.scm dirs.scm priorque.scm queue.scm\
	srfi.scm srfi-1.scm\
	pnm.scm http-cgi.scm htmlform.scm html4each.scm db2html.scm uri.scm\
	dft.scm solid.scm random.scm randinex.scm obj2str.scm ncbi-dna.scm\
	minimize.scm factor.scm determ.scm daylight.scm colornam.scm\
	mkclrnam.scm color.scm subarray.scm dbutil.scm array.scm transact.scm\
	arraymap.scm phil-spc.scm lineio.scm differ.scm cvs.scm tree.scm\
	coerce.scm byte.scm bytenumb.scm matfile.scm tsort.scm manifest.scm\
	peanosfc.scm linterp.scm math-integer.scm
txifiles =grapheps.txi glob.txi getparam.txi\
	vet.txi top-refs.txi hashtab.txi chap.txi comparse.txi\
	alist.txi ratize.txi modular.txi dirs.txi priorque.txi queue.txi\
	srfi.txi srfi-1.txi\
	pnm.txi http-cgi.txi htmlform.txi html4each.txi db2html.txi uri.txi\
	dft.txi solid.txi random.txi randinex.txi obj2str.txi ncbi-dna.txi\
	minimize.txi factor.txi determ.txi daylight.txi colornam.txi\
	mkclrnam.txi color.txi subarray.txi dbutil.txi array.txi transact.txi\
	arraymap.txi phil-spc.txi lineio.txi differ.txi cvs.txi tree.txi\
	coerce.txi byte.txi bytenumb.txi matfile.txi tsort.txi manifest.txi\
	peanosfc.txi linterp.txi math-integer.txi
% = `echo $(txiscms) | sed 's%.scm%.txi%g'`

texifiles = schmooz.texi indexes.texi object.texi format.texi limit.texi \
	 fdl.texi
docfiles = ANNOUNCE README COPYING FAQ slib.1 slib.info slib.texi version.txi\
	 ChangeLog $(texifiles) $(txifiles)
mkfiles = Makefile require.scm Template.scm syncase.sh mklibcat.scm \
	Bev2slib.scm slib.spec slib.sh grapheps.ps
ifiles = bigloo.init chez.init elk.init macscheme.init mitscheme.init \
	scheme2c.init scheme48.init gambit.init t3.init \
	vscm.init scm.init scsh.init pscheme.init STk.init \
	RScheme.init DrScheme.init umbscheme.init guile.init jscheme.init
tfiles = macrotst.scm dwindtst.scm formatst.scm
sfiles = $(ffiles) $(lfiles) $(revfiles) $(afiles) $(scfiles) $(efiles) \
	$(rfiles) $(gfiles) $(scafiles) $(txiscms) $(srfiles)
allfiles = $(docfiles) $(mkfiles) $(ifiles) $(sfiles) $(tfiles) $(bfiles) \
	slib.doc clrnamdb.scm

pinstall:	slib.1
	test -d $(mandir) || mkdir $(mandir)
	test -d $(man1dir) || mkdir $(man1dir)
	-cp slib.1 $(man1dir)

install:	pinstall clrnamdb.scm
	test -d $(libdir) || mkdir $(libdir)
	test -d $(libslibdir) || mkdir $(libslibdir)
	-cp  $(ifiles) $(sfiles) $(bfiles) $(mkfiles) clrnamdb.scm $(libslibdir)
	test -d $(bindir) || mkdir $(bindir)
	echo '#! /bin/sh'			 > $(bindir)slib
	echo SCHEME_LIBRARY_PATH=$(libslibdir)  >> $(bindir)slib
	echo export SCHEME_LIBRARY_PATH		>> $(bindir)slib
	echo VERSION=$(VERSION)			>> $(bindir)slib
	echo "S48_VICINITY=\"$(S48LIB)\";export S48_VICINITY" >> $(bindir)slib
	cat slib.sh				>> $(bindir)slib
	chmod +x $(bindir)slib

uninstall:
	-(cd $(libslibdir); rm $(ifiles) $(sfiles) $(bfiles) $(mkfiles) clrnamdb.scm)
	-rm $(bindir)slib
	-rm $(man1dir)slib.1

slib.doc:	slib.1
	nroff -man $< | ul -tunknown >$@

#### Stuff for maintaining SLIB below ####

ver = $(VERSION)
version.txi:	Makefile
	echo @set SLIBVERSION $(ver) > version.txi
	echo @set SLIBDATE `date +"%B %Y"` >> version.txi

scheme = scm

collect.sc:
	echo "(require 'macros-that-work)" > collect.sc
	echo "(require 'pprint-file)" >> collect.sc
	echo "(require 'yasos)" >> collect.sc
	echo "(pprint-filter-file \"collect.scm\" macwork:expand \"collectx.scm\")" >> collect.sc
	echo "(slib:exit #t)" >> collect.sc

collectx.scm: collect.scm macwork.scm collect.sc
	$(scheme) < collect.sc

$(txifiles):	$(txiscms) schmooz.scm
	$(scheme) -rschmooz -e'(schmooz "slib.texi")'

dvi:	slib.dvi
slib.dvi:	version.txi slib.texi $(txifiles) $(texifiles)
	texi2dvi -b -c $(srcdir)slib.texi
slib.fn:
	tex $(srcdir)slib.texi \
	$(srcdir)schmooz.texi
xdvi:	slib.dvi
	xdvi -s 4 slib.dvi

pdf:	$(htmldir)slib.pdf
$(htmldir)slib.pdf:	version.txi slib.texi $(txifiles) $(texifiles)
#	dvipdf slib.dvi # doesn't have links!
	texi2pdf -b -c $(srcdir)slib.texi
	mv slib.pdf $(htmldir)
xpdf:	$(htmldir)slib.pdf
	xpdf $(htmldir)slib.pdf

TEXI2HTML = /usr/local/bin/texi2html -split -verbose
slib_toc.html:	version.txi slib.texi $(txifiles) $(texifiles)
	${TEXI2HTML} slib.texi
html:	$(htmldir)slib_toc.html
$(htmldir)slib_toc.html:	slib_toc.html Makefile
	-rm -f slib_stoc.html
	if [ -f $(PREVDOCS)slib_toc.html ]; \
		then hitch $(PREVDOCS)slib_\*.html slib_\*.html $(htmldir); \
		else cp slib_*.html $(htmldir);fi

slib$(VERSION).info:	version.txi slib.texi $(txifiles) $(texifiles)
	makeinfo slib.texi --no-warn --no-split -o slib.info
	mv slib.info slib$(VERSION).info
slib.info:	slib$(VERSION).info
	if [ -f $(PREVDOCS)slib.info ];\
		then infobar $(PREVDOCS)slib.info slib$(VERSION).info slib.info;\
		else cp slib$(VERSION).info slib.info;fi
info:	installinfo
installinfo:	$(infodir)slib.info
$(infodir)slib.info:	slib.info
	cp -p slib.info $(infodir)slib.info
	-install-info $(infodir)slib.info $(infodir)dir
	-rm $(infodir)slib.info.gz
infoz:	installinfoz
installinfoz:	$(infodir)slib.info.gz
$(infodir)slib.info.gz:	$(infodir)slib.info
	gzip -f $(infodir)slib.info

docs:	$(infodir)slib.info.gz $(htmldir)slib_toc.html slib.dvi \
	 $(htmldir)slib.pdf slib.doc
	xdvi -s 4 slib.dvi

makedev = make -f $(HOME)/makefile.dev
CHPAT=$(HOME)/bin/chpat
RSYNC=rsync --rsync-path=bin/rsync -bav
UPLOADEE=swissnet_upload
dest = $(HOME)/dist/
DOSCM = /misc/usb1/scm/

temp/slib:	$(allfiles)
	-rm -rf temp
	mkdir temp
	mkdir temp/slib
	ln  $(allfiles) temp/slib

infotemp/slib:	slib.info
	-rm -rf infotemp
	mkdir infotemp
	mkdir infotemp/slib
	ln slib.info slib.info-* infotemp/slib
#For change-barred HTML.
prevdocs:	$(PREVDOCS)slib_toc.html $(PREVDOCS)slib.info
$(PREVDOCS)slib_toc.html:
$(PREVDOCS)slib.info:	srcdir.mk Makefile
	cd $(PREVDOCS); unzip -ao $(dest)slib*.zip
	rm $(PREVDOCS)slib/slib.info
	cd $(PREVDOCS)slib; make slib.info; make slib_toc.html
	cd $(PREVDOCS); mv -f slib/slib.info slib/*.html ./
	rm -rf $(PREVDOCS)slib

distinfo:	$(dest)slib.info.zip
$(dest)slib.info.zip:	infotemp/slib
	$(makedev) TEMP=infotemp/ DEST=$(dest) PROD=slib ver=.info zip
	rm -rf infotemp

release:	dist pdf tar.gz	# rpm
	cvs tag -F slib$(VERSION)
	cp ANNOUNCE $(htmldir)SLIB_ANNOUNCE.txt
	cp COPYING  $(htmldir)SLIB_COPYING.txt
	$(RSYNC) $(htmldir)SLIB.html $(htmldir)SLIB_ANNOUNCE.txt \
	 $(htmldir)SLIB_COPYING.txt $(UPLOADEE):public_html/
	$(RSYNC) $(dest)README $(dest)slib$(VERSION).zip \
	 $(dest)slib$(VERSION).tar.gz $(htmldir)slib.pdf \
	 $(dest)slib-$(VERSION)-$(RELEASE).noarch.rpm \
	 $(dest)slib-$(VERSION)-$(RELEASE).src.rpm $(UPLOADEE):dist/
#	upload $(dest)README $(dest)slib$(VERSION).zip ftp.gnu.org:gnu/jacal/
#	$(MAKE) indiana
indiana:
	upload $(dest)slib$(VERSION).zip ftp@ftp.cs.indiana.edu:/pub/scheme-repository/incoming
	echo -e \
	'I have uploaded slib$(VERSION).zip to ftp.cs.indiana.edu:/pub/scheme-repository/incoming\n' \
	'for placement into ftp.cs.indiana.edu:/pub/scheme-repository/code/lib/' \
	 | mail -s 'SLIB upload' -b jaffer scheme-repository-request@cs.indiana.edu

postnews:
	echo -e "Newsgroups: comp.lang.scheme\n" | cat - ANNOUNCE | \
	inews -h -O -S \
	-f "announce@voluntocracy.org (Aubrey Jaffer & Radey Shouman)" \
	 -t "SLIB$(VERSION) Released" -d world

upzip:	$(HOME)/pub/slib.zip
	$(RSYNC) $(HOME)/pub/slib.zip $(UPLOADEE):pub/

dist:	$(dest)slib$(VERSION).zip
$(dest)slib$(VERSION).zip:	temp/slib
	$(makedev) DEST=$(dest) PROD=slib ver=$(VERSION) zip

tar.gz:	$(dest)slib$(VERSION).tar.gz
$(dest)slib$(VERSION).tar.gz:	temp/slib
	$(makedev) DEST=$(dest) PROD=slib ver=$(VERSION) tar.gz

rpm:	pubzip
#$(dest)slib-$(VERSION)-$(RELEASE).noarch.rpm:	$(dest)slib$(VERSION).zip
	cp $(HOME)/pub/slib.zip $(rpm_prefix)SOURCES/slib$(VERSION).zip
	rpmbuild -ba slib.spec	# --clean
	rm $(rpm_prefix)SOURCES/slib$(VERSION).zip
	mv $(rpm_prefix)RPMS/noarch/slib-$(VERSION)-$(RELEASE).noarch.rpm \
	   $(rpm_prefix)SRPMS/slib-$(VERSION)-$(RELEASE).src.rpm $(dest)

shar:	slib.shar
slib.shar:	temp/slib
	$(makedev) PROD=slib shar
dclshar:	slib.com
com:	slib.com
slib.com:	temp/slib
	$(makedev) PROD=slib com
zip:	slib.zip
slib.zip:	temp/slib
	$(makedev) DEST=../ PROD=slib zip
doszip:	$(DOSCM)dist/slib$(VERSION).zip
$(DOSCM)dist/slib$(VERSION).zip:	temp/slib
	$(makedev) DEST=$(DOSCM)dist/ PROD=slib ver=$(VERSION) zip
	zip -d $(DOSCM)dist/slib$(VERSION).zip slib/slib.info
pubzip:	temp/slib
	$(makedev) DEST=$(HOME)/pub/ PROD=slib zip

diffs:	pubdiffs
pubdiffs:	temp/slib
	$(makedev) DEST=$(HOME)/pub/ PROD=slib pubdiffs
distdiffs:	temp/slib
	$(makedev) DEST=$(dest) PROD=slib ver=$(ver) distdiffs
announcediffs:	temp/slib
	$(makedev) DEST=$(dest) PROD=slib ver=$(VERSION) announcediffs

psdfiles=COPYING.psd README.psd cmuscheme.el comint.el instrum.scm pexpr.scm \
	primitives.scm psd-slib.scm psd.el read.scm runtime.scm version.scm
psdocfiles=article.bbl article.tex manual.bbl manual.tex quick-intro.tex

psdtemp/slib:
	-rm -rf psdtemp
	mkdir psdtemp
	mkdir psdtemp/slib
	mkdir psdtemp/slib/psd
	cd psd; ln $(psdfiles) ../psdtemp/slib/psd
	mkdir psdtemp/slib/psd/doc
	cd psd/doc; ln $(psdocfiles) ../../psdtemp/slib/psd/doc

psdist:	$(dest)slib-psd.tar.gz
$(dest)slib-psd.tar.gz:	psdtemp/slib
	$(makedev) DEST=$(dest) PROD=slib ver=-psd tar.gz TEMP=psdtemp/

new:
	echo `date -I` \ Aubrey Jaffer \ \<`whoami`@`hostname`\>> change
	echo>> change
	echo \	\* require.scm \(*slib-version*\): Bumped from $(VERSION) to $(ver).>>change
	echo>> change
	cat ChangeLog >> change
	mv -f change ChangeLog
	$(CHPAT) slib$(VERSION) slib$(ver) ANNOUNCE FAQ ../scm/ANNOUNCE \
		../jacal/ANNOUNCE ../wb/README ../wb/ANNOUNCE \
		../synch/ANNOUNCE \
		$(htmldir)README.html ../dist/README \
		$(htmldir)JACAL.html \
		$(htmldir)SCM.html \
		$(htmldir)SIMSYNCH.html ../scm/scm.texi \
		$(DOSCM)dist/install.bat $(DOSCM)dist/makefile \
		$(DOSCM)dist/mkdisk.bat
	$(CHPAT) slib-$(VERSION) slib-$(ver) ANNOUNCE FAQ ../scm/ANNOUNCE \
		../jacal/ANNOUNCE ../wb/README ../wb/ANNOUNCE \
		../synch/ANNOUNCE \
		$(htmldir)README.html ../dist/README \
		$(htmldir)JACAL.html \
		$(htmldir)SCM.html \
		$(htmldir)SIMSYNCH.html ../scm/scm.texi \
		$(DOSCM)dist/install.bat $(DOSCM)dist/makefile \
		$(DOSCM)dist/mkdisk.bat
	$(CHPAT) $(VERSION) $(ver) README slib.texi require.scm Makefile \
		 $(htmldir)SLIB.html slib.spec scheme48.init
	cvs commit -lm '(*slib-version*): Bumped from $(VERSION) to $(ver).'
	cvs tag -lF slib$(ver)

tagfiles = README version.txi slib.texi $(texifiles) \
	$(mkfiles) $(sfiles) $(bfiles) $(tfiles) $(ifiles)
# README and $(ifiles) cause semgentation faults in ETAGS for Emacs version 19.
tags:	$(tagfiles)
	etags $(tagfiles)
test:	$(sfiles)
	$(scheme) Template.scm $(sfiles)
rights:
	$(scheme) -ladmin -e"(admin:check-all)" $(sfiles) $(tfiles) \
		$(bfiles) $(ifiles)
report:
	$(scheme) -e"(slib:report #t)"
clean:
	-rm -f *~ *.bak *.orig *.rej core a.out *.o \#*
	-rm -rf *temp
distclean:	realclean
realclean:
	-rm -f *~ *.bak *.orig *.rej TAGS core a.out *.o \#*
	-rm -f slib.info* slib.?? slib.???
	-rm -rf *temp
realempty:	temp/slib
	-rm -f $(allfiles)
