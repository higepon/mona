TARGET = mona.img

ROOT_FILES = `cd root; ls *.*`
SVR_FILES  = `cd root; ls SERVERS/*.*`
APP_FILES  = `cd root; ls APPS/*.*`

all:
	cp fat_template.img $(TARGET)
	./fat_write --mbr $(TARGET) firstboot.bin
	./fat_write --mkdir $(TARGET) SERVERS
	./fat_write --mkdir $(TARGET) APPS
	@for file in $(ROOT_FILES); do echo "Writing: "$$file"..."; ./fat_write $(TARGET) root/$$file $$file; done
	@for file in $(SVR_FILES) ; do echo "Writing: "$$file"..."; ./fat_write $(TARGET) root/$$file $$file; done
	@for file in $(APP_FILES) ; do echo "Writing: "$$file"..."; ./fat_write $(TARGET) root/$$file $$file; done
	@echo "Done."

write:
	dd if=$(TARGET) of=/dev/fd0

clean:
	rm -f $(TARGET)
	rm -f root/KERNEL.IMG
	rm -f root/MONA-12.MNF
	rm -rf root/APPS/*.*
	rm -rf root/SERVERS/*.*
