TARGET = mona.iso
FD_IMG = mona.img

ROOT_FILES     = `cd root; ls *.*`
LIB_FILES      = `cd root; ls LIBS/*.*`

all:
# create mona.img
	@echo "Creating mona.img"
	cd root/SERVERS; rm -f PROCESS.BIN FILE.BIN MONITOR.BIN
	cp fat_template.img $(FD_IMG)
	./fat_write $(FD_IMG) --mbr firstboot.bin
	./fat_write $(FD_IMG) --mkdir SERVERS
	./fat_write $(FD_IMG) --mkdir APPS
	./fat_write $(FD_IMG) --mkdir LIBS
	./fat_write $(FD_IMG) ../src/servers/file/FILE.BIN FILE.BIN
	./fat_write $(FD_IMG) ../src/servers/process/PROCESS.BIN PROCESS.BIN
	./fat_write $(FD_IMG) ../src/servers/moniter/MONITOR.BIN MONITOR.BIN
	./fat_write $(FD_IMG) ../src/servers/moniter/MONITOR.BIN SERVERS/MONITOR.BIN
	./fat_write $(FD_IMG) ../src/servers/elf/ELF.BN5 SERVERS/ELF.BN5
	./fat_write $(FD_IMG) ../src/servers/pe/PE.BN5 SERVERS/PE.BN5
	./fat_write $(FD_IMG) ../src/servers/key/KEYBDMNG.EX5 SERVERS/KEYBDMNG.EX5
	./fat_write $(FD_IMG) ../src/servers/mouse/MOUSE.EX5 SERVERS/MOUSE.EX5
	./fat_write $(FD_IMG) ../src/servers/shell/OLDSHELL.EX5 SERVERS/OLDSHELL.EX5
	@echo $(ROOT_FILES)
	@echo $(LIB_FILES)
	@for file in $(ROOT_FILES) $(LIB_FILES); \
	do \
	  echo "Writing: "$$file"..."; ./fat_write $(FD_IMG) root/$$file $$file; \
	done
	@echo "Done."
# create mona.iso
	@echo "Creating mona.img"
	cp $(FD_IMG) iso
	mkisofs -v -iso-level 1 -b $(FD_IMG) -o $(TARGET) iso
	rm -f iso/$(FD_IMG)
	@echo "Done."

write:
	dd if=$(TARGET) of=/dev/fd0

clean:
	rm -f $(TARGET) $(FD_IMG)
	rm -f root/*.BIN
	rm -f root/MONA12.M*
	rm -rf root/APPS/*.*
	rm -rf root/APPS/MONAFRMS
	rm -rf root/APPS/BAYGUI
	rm -rf root/SERVERS/*.*
	rm -rf root/LIBS
	rm -f iso/*.BIN
	rm -f iso/MONA12.M*
	rm -rf iso/APPS/*.*
	rm -rf iso/APPS/MONAFRMS
	rm -rf iso/APPS/BAYGUI
	rm -rf iso/SERVERS/*.*
	rm -rf iso/LIBS
