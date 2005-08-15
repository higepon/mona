all:
	cd Graphics/libjpegls && touch dependencies && make && make install
	cd X/gui && touch dependencies && make && make install
	cd Devel/stlport && make && make install
	cd Graphics/libgui && touch dependencies && make && make install
	cd Shells/shell && touch dependencies && make && make install
	cd X/monafile && touch dependencies && make && make install
	cd Graphics/libbaygui && touch dependencies && make && make install
	cd Shells/gshell && touch dependencies && make && make install
	cd X/glaunch && touch dependencies && make && make install
	cd ../MonaNew/tool/mkimg && make

clean:
	cd Graphics/libjpegls && make clean
	cd X/gui && make clean
	cd Graphics/libgui && make clean
	cd Shells/shell && make clean
	cd X/monafile && make clean
	cd Graphics/libbaygui && make clean
	cd Shells/gshell && make clean
	cd X/glaunch && make clean
