TARGET = mona.img

ROOT_FILES = `cd root; ls *.*`
SVR_FILES  = `cd root; ls SERVERS/*.*`
APP_FILES  = `cd root; ls APPS/*.E* APPS/*.LIB APPS/*.WRP APPS/*.MSH`
LIB_FILES  = `cd root; ls LIBS/*.*`

all:
	cp fat_template.img $(TARGET)
	./fat_write $(TARGET) --mbr firstboot.bin
	./fat_write $(TARGET) --mkdir SERVERS
	./fat_write $(TARGET) --mkdir APPS
	./fat_write $(TARGET) --mkdir LIBS
	@for file in $(ROOT_FILES) $(SVR_FILES) $(APP_FILES) $(LIB_FILES); \
	do \
	  echo "Writing: "$$file"..."; ./fat_write $(TARGET) root/$$file $$file; \
	done
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
	rm -rf root/LIBS
