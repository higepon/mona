TARGET = mona.img

ROOT_FILES = `cd root; ls *.*`
SVR_FILES  = `cd root; ls SERVERS/*.BN2`
BIN_FILES  = `cd root; ls SERVERS/*.BIN`
APP_FILES  = `cd root; ls APPS/*.EL* APPS/*.LIB APPS/*.WRP APPS/*.MSH`

all:
	cp fat_template.img $(TARGET)
	./fat_write $(TARGET) --mbr firstboot.bin
	./fat_write $(TARGET) --mkdir SERVERS
	./fat_write $(TARGET) --mkdir APPS
	@for file in $(ROOT_FILES); do echo "Writing: "$$file"..."; ./fat_write $(TARGET) root/$$file $$file; done
	@for file in $(SVR_FILES) ; do echo "Writing: "$$file"..."; ./fat_write $(TARGET) root/$$file $$file; done
	@for file in $(BIN_FILES) ; do echo "Writing: "$$file"..."; ./fat_write $(TARGET) root/$$file $$file; done
	@for file in $(APP_FILES) ; do echo "Writing: "$$file"..."; ./fat_write $(TARGET) root/$$file $$file; done
	@cd root; for dir in APPS/*.APP; \
	do \
	  echo "Creating: "$$dir"..."; ../fat_write ../$(TARGET) --mkdir $$dir; \
	  for file in $$dir/*.*; do echo "Writing: "$$file"..."; ../fat_write ../$(TARGET) $$file $$file; done; \
	done
	@echo "Done."

write:
	dd if=$(TARGET) of=/dev/fd0

clean:
	rm -f $(TARGET)
	rm -f root/KERNEL.IMG
	rm -f root/MONA-12.M*
	rm -rf root/APPS/*.*
	rm -rf root/SERVERS/*.*
