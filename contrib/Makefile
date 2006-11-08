ifndef $(MONADIR)
#export MONADIR=$(PWD)/../new-file-server-mona
export MONADIR=$(PWD)/../mona
endif

all:
### basic libraries
	cd Graphics/libjpegls && touch dependencies && $(MAKE) && $(MAKE) install
### servers
	cd X/gui && touch dependencies && $(MAKE) && $(MAKE) install
	cd Sysutils/libPci && touch dependencies && $(MAKE) && $(MAKE) install
	cd Net/NetServer && $(MAKE) && $(MAKE) install
### libraries
	cd Graphics/Mesa && touch dependencies && $(MAKE) && $(MAKE) install
	cd Graphics/libgui && touch dependencies && $(MAKE) && $(MAKE) install
	cd Graphics/libbaygui && touch dependencies && $(MAKE) && $(MAKE) install
	cd Net/libnet && $(MAKE) && $(MAKE) install
### non-gui applications
	cd Emulators/infones && touch dependencies && $(MAKE) && $(MAKE) install
	cd Emulators/monamona && touch dependencies && $(MAKE) && $(MAKE) install
	cd Graphics/jpegview && touch dependencies && $(MAKE) && $(MAKE) install
	cd Graphics/Mesa-gears && touch dependencies && $(MAKE) && $(MAKE) install
	cd Misc/helloworld && touch dependencies && $(MAKE) && $(MAKE) install
	cd Misc/stdoutclienttest && touch dependencies && $(MAKE) && $(MAKE) install
	cd Misc/stdoutservertest && touch dependencies && $(MAKE) && $(MAKE) install
	cd Multimedia/monapeg && touch dependencies && $(MAKE) && $(MAKE) install
	cd Multimedia/monappt && touch dependencies && $(MAKE) && $(MAKE) install
### MonaForms applications
	cd Shells/mshell && touch dependencies && $(MAKE) && $(MAKE) install
	cd X/monafile && touch dependencies && $(MAKE) && $(MAKE) install
	cd Net/monadat && touch dependencies && $(MAKE) && $(MAKE) install
### BayGUI applications
	cd Shells/gshell && touch dependencies && $(MAKE) && $(MAKE) install
	cd X/glaunch && touch dependencies && $(MAKE) && $(MAKE) install
	cd Misc/gnote && touch dependencies && $(MAKE) && $(MAKE) install
	cd Multimedia/xmonapeg && touch dependencies && $(MAKE) && $(MAKE) install
#	cd Emulators/monasq && touch dependencies && $(MAKE) && $(MAKE) install
### Network applications
	cd Net/libnet/sample/giko && $(MAKE) && $(MAKE) install
### Append
	cd Benchmarks/gfire && $(MAKE) && $(MAKE) install
	cd Benchmarks/gnoiz2bg && $(MAKE) && $(MAKE) install
	cd Games/ghebtn && $(MAKE) && $(MAKE) install
	cd Games/gnullpo && $(MAKE) && $(MAKE) install
	cd Games/gtetris && $(MAKE) && $(MAKE) install
	cd Games/mblupin && $(MAKE) && $(MAKE) install
	cd Games/mine && $(MAKE) && $(MAKE) install
	cd Games/mtorz && $(MAKE) && $(MAKE) install
	cd Graphics/bitmap && $(MAKE) && $(MAKE) install
	cd Sysutils/gclock && $(MAKE) && $(MAKE) install
	cd Sysutils/gtimer && $(MAKE) && $(MAKE) install
	cd Sysutils/startx && $(MAKE) && $(MAKE) install
### $(MAKE) disk images
	cd $(MONADIR)/tool/mkimg && $(MAKE)

clean:
	cd Emulators/infones && $(MAKE) clean
	cd Emulators/monamona && $(MAKE) clean
	cd Graphics/jpegview && $(MAKE) clean
	cd Graphics/libbaygui && $(MAKE) clean
	cd Graphics/libjpegls && $(MAKE) clean
	cd Graphics/libgui && $(MAKE) clean
	cd Graphics/Mesa && $(MAKE) clean
	cd Graphics/Mesa-gears && $(MAKE) clean
	cd Misc/helloworld && $(MAKE) clean
	cd Misc/stdoutservertest && $(MAKE) clean
	cd Misc/stdoutclienttest && $(MAKE) clean
	cd Misc/gnote && $(MAKE) clean
	cd Multimedia/monapeg && $(MAKE) clean
	cd Multimedia/monappt && $(MAKE) clean
	cd Multimedia/xmonapeg && $(MAKE) clean
	cd Net/libnet && $(MAKE) clean
	cd Net/monadat && $(MAKE) clean
	cd Net/NetServer && $(MAKE) clean
	cd Shells/gshell && $(MAKE) clean
	cd Shells/mshell && $(MAKE) clean
	cd Sysutils/libPci && $(MAKE) clean
	cd X/gui && $(MAKE) clean
	cd X/glaunch && $(MAKE) clean
	cd X/monafile && $(MAKE) clean
	cd Benchmarks/gfire && $(MAKE) clean
	cd Benchmarks/gnoiz2bg && $(MAKE) clean
	cd Games/ghebtn && $(MAKE) clean
	cd Games/gnullpo && $(MAKE) clean
	cd Games/gtetris && $(MAKE) clean
	cd Games/mblupin && $(MAKE) clean
	cd Games/mine && $(MAKE) clean
	cd Games/mtorz && $(MAKE) clean
	cd Graphics/bitmap && $(MAKE) clean
	cd Sysutils/gclock && $(MAKE) clean
	cd Sysutils/gtimer && $(MAKE) clean
	cd Sysutils/startx && $(MAKE) clean
