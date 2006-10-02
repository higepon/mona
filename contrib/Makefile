ifndef $(MONADIR)
#export MONADIR=$(PWD)/../new-file-server-mona
export MONADIR=$(PWD)/../mona
endif

all:
### basic libraries
	cd Graphics/libjpegls && touch dependencies && make && make install
### servers
	cd X/gui && touch dependencies && make && make install
	cd Sysutils/libPci && touch dependencies && make && make install
	cd Net/NetServer && make && make install
### libraries
	cd Graphics/Mesa && touch dependencies && make && make install
	cd Graphics/libgui && touch dependencies && make && make install
	cd Graphics/libbaygui && touch dependencies && make && make install
	cd Net/libnet && make && make install
### non-gui applications
	cd Emulators/infones && touch dependencies && make && make install
	cd Emulators/monamona && touch dependencies && make && make install
	cd Graphics/jpegview && touch dependencies && make && make install
	cd Graphics/Mesa-gears && touch dependencies && make && make install
	cd Misc/helloworld && touch dependencies && make && make install
	cd Misc/stdoutclienttest && touch dependencies && make && make install
	cd Misc/stdoutservertest && touch dependencies && make && make install
	cd Multimedia/monapeg && touch dependencies && make && make install
	cd Multimedia/monappt && touch dependencies && make && make install
### MonaForms applications
	cd Shells/mshell && touch dependencies && make && make install
	cd X/monafile && touch dependencies && make && make install
	cd Net/monadat && touch dependencies && make && make install
### BayGUI applications
	cd Shells/gshell && touch dependencies && make && make install
	cd X/glaunch && touch dependencies && make && make install
	cd Misc/gnote && touch dependencies && make && make install
	cd Multimedia/xmonapeg && touch dependencies && make && make install
### Network applications
	cd Net/libnet/sample/giko && make && make install
### Append
	cd Benchmarks/gfire && make && make install
	cd Benchmarks/gnoiz2bg && make && make install
	cd Games/ghebtn && make && make install
	cd Games/gnullpo && make && make install
	cd Games/mblupin && make && make install
	cd Games/mine && make && make install
	cd Games/mtorz && make && make install
	cd Graphics/bitmap && make && make install
	cd Sysutils/gclock && make && make install
	cd Sysutils/gtimer && make && make install
	cd Sysutils/startx && make && make install
### make disk images
	cd $(MONADIR)/tool/mkimg && make

clean:
	cd Emulators/infones && make clean
	cd Emulators/monamona && make clean
	cd Graphics/jpegview && make clean
	cd Graphics/libbaygui && make clean
	cd Graphics/libjpegls && make clean
	cd Graphics/libgui && make clean
	cd Graphics/Mesa && make clean
	cd Graphics/Mesa-gears && make clean
	cd Misc/helloworld && make clean
	cd Misc/stdoutservertest && make clean
	cd Misc/stdoutclienttest && make clean
	cd Misc/gnote && make clean
	cd Multimedia/monapeg && make clean
	cd Multimedia/monappt && make clean
	cd Multimedia/xmonapeg && make clean
	cd Net/libnet && make clean
	cd Net/monadat && make clean
	cd Net/NetServer && make clean
	cd Shells/gshell && make clean
	cd Shells/mshell && make clean
	cd Sysutils/libPci && make clean
	cd X/gui && make clean
	cd X/glaunch && make clean
	cd X/monafile && make clean
	cd Benchmarks/gfire && make clean
	cd Benchmarks/gnoiz2bg && make clean
	cd Games/ghebtn && make clean
	cd Games/gnullpo && make clean
	cd Games/mblupin && make clean
	cd Games/mine && make clean
	cd Games/mtorz && make clean
	cd Graphics/bitmap && make clean
	cd Sysutils/gclock && make clean
	cd Sysutils/gtimer && make clean
	cd Sysutils/startx && make clean
