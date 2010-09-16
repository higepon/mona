SRCTOP=../..
include $(SRCTOP)/Makefile.inc

all:
	make clean
	MONADIR=$(MONADIR) perl mkimg.pl
	make iso
	make fat32
	cp mona.img test.img

clean:
	rm -f mona.img
	rm -f $(MONADIR)/bin/mona.img
	rm -f hdd.img

fat32:
	rm -rf ./tmp
	mkfs.vfat -C -F32 hdd.img 100000 # 48MB
	mkdir -p ./tmp
	sudo mount ./hdd.img tmp/ -t vfat -o loop
	for i in `seq 1 18`; do echo "Hello" > test$$i.txt; sudo mv -f test$$i.txt tmp; done
	touch empty.txt; sudo mv -f empty.txt tmp
#	touch empty2.txt; sudo mv -f empty2.txt tmp
#	(cd tmp && echo h | sudo tee empty.txt)
	seq 1 1000 > hige.txt; sudo mv -f hige.txt tmp
	sudo umount -l tmp/

iso:
	$(INSTALL) -p mona.img $(MONADIR)/bin
	$(INSTALL) -p AUTOEXEC.MSH $(MONADIR)/bin
	$(INSTALL) -p MONITOR.CFG $(MONADIR)/bin
	$(INSTALL) -p MONA.CFG $(MONADIR)/bin
	cd $(MONADIR) && mkisofs -v -iso-level 1 -joliet -b mona.img -o mona.iso bin
	mv $(MONADIR)/mona.iso .
	rm -f $(MONADIR)/bin/mona.img

distclean: clean
