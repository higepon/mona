all:
	make clean
	perl mkimg.pl
	make iso

clean:
	rm -f mona.img
	rm -f $(MONADIR)/bin/mona.img

iso:
	cp mona.img $(MONADIR)/bin
	cp AUTOEXEC.MSH $(MONADIR)/bin
	cp MONITOR.CFG $(MONADIR)/bin
	cp MONA.CFG $(MONADIR)/bin
	cd $(MONADIR) && mkisofs -v -iso-level 1 -b mona.img -o mona.iso bin
	mv $(MONADIR)/mona.iso .
	rm -f $(MONADIR)/bin/mona.img

distclean: clean
