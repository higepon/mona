SRCTOP=../..
include $(SRCTOP)/Makefile.inc

all:
	make clean
	MONADIR=$(MONADIR) perl mkimg.pl
	make iso

clean:
	rm -f mona.img
	rm -f $(MONADIR)/bin/mona.img

iso:
	$(INSTALL) -p mona.img $(MONADIR)/bin
	$(INSTALL) -p AUTOEXEC.MSH $(MONADIR)/bin
	$(INSTALL) -p MONITOR.CFG $(MONADIR)/bin
	$(INSTALL) -p MONA.CFG $(MONADIR)/bin
	cd $(MONADIR) && mkisofs -v -iso-level 1 -b mona.img -o mona.iso bin
	mv $(MONADIR)/mona.iso .
	rm -f $(MONADIR)/bin/mona.img

distclean: clean
