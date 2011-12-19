SRCTOP=../..
include $(SRCTOP)/Makefile.inc

FAT32IMG=fat32_user.img
TESTIMG=fat32_test.img

all:
	make clean
	MONADIR=$(MONADIR) perl mkimg.pl
	make iso
	mosh check_image_backup.scm
	(test -f ./fat32_user.img && make backup_user) || true
	make fat32
	cp mona.img test.img

clean:
	rm -f mona.img
	rm -f $(MONADIR)/bin/mona.img

create_user_image_once:
	rm -f $(FAT32IMG)
	bunzip2 -dc fat32empty.img.bz2 > $(FAT32IMG)
	mmd -i $(FAT32IMG) bin
	mmd -i $(FAT32IMG) lib
	mmd -i $(FAT32IMG) test
	mcopy -/ -i fat32_user.img ../../user/bin/* ::bin/
	mcopy -/ -i fat32_user.img ../../user/lib/* ::lib/
	mcopy -/ -i fat32_user.img ../../user/test/* ::test/
	mmd -i $(FAT32IMG) temp
	@if [ ! -f $@ ]; then touch $@; fi

backup_user:
	mkdir -p tmp
	mcopy -/ -n -i $(FAT32IMG) :: tmp/
#	(test -d ~/Dropbox/mona_images/ && tar czvf ~/Dropbox/mona_images/backup.`date '+%Y_%m_%d_%H_%M_%S'`.tar.gz tmp)
	cp tmp/bin/*.SPS ../../user/bin/ || true
	cp -r tmp/lib/* ../../user/lib || true
	cp -r tmp/test/* ../../user/test || true
	git add ../../user/bin/* || true
	git add ../../user/lib/*.SLS || true
	git add ../../user/lib/*/*.SLS || true
	git add ../../user/test/*.SPS || true

# restore_backup_user:
# 	mkdir work
# 	(cd work && tar zvxf $(BACKUP) && cd tmp && for i in `ls`; do mcopy -/ -n -i $(FAT32IMG) $$i; done)
# 	rm -r work

create_test_image_once:
	bunzip2 -dc fat32empty.img.bz2 > $(TESTIMG)
	@if [ ! -f $@ ]; then touch $@; fi

setup_test_image: create_test_image_once
	for i in `mdir -b -i fat32_test.img`; do mdeltree -i fat32_test.img $$i; done
	for i in `$(SEQ_COMMAND) 1 18`; do echo "Hello" > test$$i.txt; mcopy -b -i $(TESTIMG) test$$i.txt ::test$$i.txt; done
	touch empty.txt; mcopy -i $(TESTIMG) ./empty.txt ::
	mmd -i $(TESTIMG) subdir
	echo "Hello" > subdir.txt %% mmove -i $(TESTIMG) subdir.txt ::subdir
	mcopy -i $(TESTIMG) show-words.scm ::subdir
	mcopy -i $(TESTIMG) words.scm ::subdir
	echo "Hello World" > this_is_very_long_file_name.txt; mcopy -i $(TESTIMG) this_is_very_long_file_name.txt ::subdir
	$(SEQ_COMMAND) 1 1000 > hige.txt && mcopy -i $(TESTIMG) hige.txt ::

fat32: setup_test_image create_user_image_once

iso:
	$(INSTALL) -p mona.img $(MONADIR)/bin
	$(INSTALL) -p AUTOEXEC.MSH $(MONADIR)/bin
	$(INSTALL) -p MONITOR.CFG $(MONADIR)/bin
	$(INSTALL) -p MONA.CFG $(MONADIR)/bin
	cd $(MONADIR) && mkisofs -iso-level 1 -joliet -b mona.img -o mona.iso bin
	mv $(MONADIR)/mona.iso .
	rm -f $(MONADIR)/bin/mona.img

distclean: clean
