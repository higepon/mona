SRCTOP=../..
include $(SRCTOP)/Makefile.inc

all:
	make clean
	MONADIR=$(MONADIR) perl mkimg.pl
	make iso
	(test -f ./fat32_user.img && make backup_user) || true
	make fat32
	cp mona.img test.img

clean:
	rm -f mona.img
	rm -f $(MONADIR)/bin/mona.img
	rm -f fat32_test.img

create_user_image_once:
	rm -f fat32_user.img
	rm -rf tmp
	mkdir tmp
	mkfs.vfat -C -F32 fat32_user.img 100000
	sudo mount ./fat32_user.img tmp/ -t vfat -o loop
	mkdir -p bin
	cp CAT.SCM bin/
	mkdir -p temp
	cp -f show-words.scm bin/
	sudo mv bin tmp
	sudo mv temp tmp
	sudo cp -f words.scm tmp
	sudo umount -l tmp/
	@if [ ! -f $@ ]; then touch $@; fi

backup_user:
	sudo mount ./fat32_user.img tmp/ -t vfat -o loop
	(test -d ~/Dropbox/mona_images/ && sudo tar czvf ~/Dropbox/mona_images/backup.`date '+%Y_%m_%d_%H_%M_%S'`.tar.gz tmp --exclude temp)
	cp tmp/bin/*.SPS ../../user/bin/
	cp -r tmp/lib/* ../../user/lib
	cp -r tmp/test/* ../../user/test
	git add ../../user/bin/* || true
	git add ../../user/lib/*.SLS || true
	git add ../../user/lib/*/*.SLS || true
	git add ../../user/test/*.SPS || true
	sudo umount -l tmp/
#	(test -d ~/Dropbox/mona_images/ && gzip -c fat32_user.img > ~/Dropbox/mona_images/fat32_user.img.`date '+%Y_%m_%d_%H%_M_%S'`.gz) || true

restore_backup_user:
	sudo mount ./fat32_user.img tmp/ -t vfat -o loop
	mkdir work
	(cd work && tar zvxf $(BACKUP) && sudo cp -r tmp/* ../tmp)
	rm -r work
	sudo umount -l tmp/

create_test_image_always:
	rm -rf tmp
	mkdir tmp
	mkfs.vfat -C -F32 fat32_test.img 100000
	sudo mount ./fat32_test.img tmp/ -t vfat -o loop
	for i in `seq 1 18`; do echo "Hello" > test$$i.txt; sudo mv -f test$$i.txt tmp; done
	touch empty.txt; sudo mv -f empty.txt tmp
	mkdir -p subdir
	echo "Hello" > subdir.txt; sudo mv -f subdir.txt subdir; done
	sudo cp -f show-words.scm subdir/
	sudo cp -f words.scm subdir/
	echo "Hello World" > this_is_very_long_file_name.txt; sudo mv -f this_is_very_long_file_name.txt subdir; done
	sudo mv subdir tmp
	seq 1 1000 > hige.txt; sudo mv -f hige.txt tmp
	sudo umount -l tmp/

fat32: create_test_image_always create_user_image_once

iso:
	$(INSTALL) -p mona.img $(MONADIR)/bin
	$(INSTALL) -p AUTOEXEC.MSH $(MONADIR)/bin
	$(INSTALL) -p MONITOR.CFG $(MONADIR)/bin
	$(INSTALL) -p MONA.CFG $(MONADIR)/bin
	cd $(MONADIR) && mkisofs -v -iso-level 1 -joliet -b mona.img -o mona.iso bin
	mv $(MONADIR)/mona.iso .
	rm -f $(MONADIR)/bin/mona.img

distclean: clean
