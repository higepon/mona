all:
# basic libraries
	cd Devel/stlport && make && make install
	cd Graphics/libjpegls && touch dependencies && make && make install
# servers
	cd X/gui && touch dependencies && make && make install
# libraries
	cd Graphics/libgui && touch dependencies && make && make install
	cd Graphics/libbaygui && touch dependencies && make && make install
# non-gui applications
	cd Games/infones && touch dependencies && make && make install
	cd Graphics/jpegdemo && touch dependencies && make && make install
	cd Multimedia/monapeg && touch dependencies && make && make install
	cd Multimedia/monappt && touch dependencies && make && make install
# MonaForms applications
	cd Net/monadat && touch dependencies && make && make install
	cd Shells/mshell && touch dependencies && make && make install
	cd X/monafile && touch dependencies && make && make install
# BayGUI applications
	cd Shells/gshell && touch dependencies && make && make install
	cd X/glaunch && touch dependencies && make && make install
# make disk images
	cd ../MonaNew/tool/mkimg && make

clean:
	cd Devel/stlport && make clean
	cd Games/infones && make clean
	cd Graphics/jpegdemo && make clean
	cd Graphics/libbaygui && make clean
	cd Graphics/libjpegls && make clean
	cd Graphics/libgui && make clean
	cd Multimedia/monapeg && make clean
	cd Shells/gshell && make clean
	cd Shells/mshell && make clean
	cd X/gui && make clean
	cd X/glaunch && make clean
	cd X/monafile && make clean
