ifndef $(MONADIR)
#export MONADIR=$(PWD)/../new-file-server-mona
export MONADIR=$(PWD)/../mona
endif

all:
# basic libraries
# stlport move to mona/tool/stlport
#	cd Devel/stlport && make install
	cd Graphics/libjpegls && touch dependencies && make && make install
# servers
	cd X/gui && touch dependencies && make && make install
# libraries
# tmpmonalibc merged in monalibc
#	cd Devel/tmpmonalibc && touch dependencies && make && make install
	cd Graphics/Mesa && touch dependencies && make && make install
	cd Graphics/libgui && touch dependencies && make && make install
	cd Graphics/libbaygui && touch dependencies && make && make install
	cd Sysutils/libPci && touch dependencies && make && make install
	cd Net/libnicdrv && touch dependencies && make && make install
	cd Net/mones2 && touch dependencies && make && make install
# non-gui applications
	cd Emulators/infones && touch dependencies && make && make install
	cd Emulators/monamona && touch dependencies && make && make install
	cd Graphics/jpegview && touch dependencies && make && make install
	cd Graphics/Mesa-gears && touch dependencies && make && make install
	cd Misc/helloworld && touch dependencies && make && make install
	cd Multimedia/monapeg && touch dependencies && make && make install
	cd Multimedia/monappt && touch dependencies && make && make install
# MonaForms applications
	cd Shells/mshell && touch dependencies && make && make install
	cd X/monafile && touch dependencies && make && make install
	cd Net/monadat && touch dependencies && make && make install
# BayGUI applications
	cd Shells/gshell && touch dependencies && make && make install
	cd X/glaunch && touch dependencies && make && make install
	cd Misc/gnote && touch dependencies && make && make install
# network support
	cd Net/NetServer && make && make install
	cd Net/libnet && make && make install
	cd Net/libnet/sample/giko && make && make install
# make disk images
	cd $(MONADIR)/tool/mkimg && make

clean:
# stlport move to mona/tool/stlport
#	cd Devel/stlport && make clean
# tmpmonalibc merged in monalibc
#	cd Devel/tmpmonalibc && make clean
	cd Emulators/infones && make clean
	cd Emulators/monamona && make clean
	cd Graphics/jpegview && make clean
	cd Graphics/libbaygui && make clean
	cd Graphics/libjpegls && make clean
	cd Graphics/libgui && make clean
	cd Graphics/Mesa && make clean
	cd Graphics/Mesa-gears && make clean
	cd Misc/helloworld && make clean
	cd Misc/gnote && make clean
	cd Multimedia/monapeg && make clean
	cd Multimedia/monappt && make clean
	cd Net/libnicdrv && make clean
	cd Net/monadat && make clean
	cd Net/mones2 && make clean
	cd Shells/gshell && make clean
	cd Shells/mshell && make clean
	cd Sysutils/libPci && make clean
	cd X/gui && make clean
	cd X/glaunch && make clean
	cd X/monafile && make clean
	cd Net/NetServer && make clean
	cd Net/libnet && make clean
