all:
	cd X/gui && touch dependencies && make && make install
	cd Graphics/libbaygui && touch dependencies && make && make install
	cd Shells/gshell && touch dependencies && make && make install
	cd X/glaunch && touch dependencies && make && make install
	cd ../MonaNew/tool/mkimg && make

clean:
	cd X/gui && make distclean
	cd Graphics/libbaygui && make distclean
	cd Shells/gshell && make clean
	cd X/glaunch && make clean
