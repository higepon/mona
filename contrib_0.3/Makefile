all:
	cd X/gui && touch dependencies && make && make install
	cd Graphics/libbaygui && touch dependencies && make && make install
	cd Shells/gshell && touch dependencies && make && make install
	cd X/glaunch && touch dependencies && make && make install

clean:
	cd X/gui && make distclean
	cd Graphics/libbaygui && make distclean
	cd Shells/gshell && make distclean
	cd X/glaunch && make distclean
