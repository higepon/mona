TARGET = mona.img

ROOT_FILES = `cd root; ls *.*`
APP_FILES  = `cd root; ls APPS/*.*`
SVR_FILES  = `cd root; ls SERVERS/*.*`

all:
	(cat firstboot.bin;dd if=fat_template.img bs=512 skip=1) > $(TARGET)
	@for file in $(ROOT_FILES); do echo "Writing: "$$file"..."; ./fat_write $(TARGET) root/$$file $$file; done
	@for file in $(APP_FILES) ; do echo "Writing: "$$file"..."; ./fat_write $(TARGET) root/$$file $$file; done
	@for file in $(SVR_FILES) ; do echo "Writing: "$$file"..."; ./fat_write $(TARGET) root/$$file $$file; done
	@echo "Done."

write:
	dd if=$(TARGET) of=/dev/fd0

clean:
	rm -f $(TARGET)
	rm -f root/KERNEL.IMG
	rm -f root/MONA-12.MNF
	rm -f root/APPS/*.*
	rm -f root/SERVERS/*.*
