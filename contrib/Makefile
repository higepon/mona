ifndef $(MONADIR)
#export MONADIR=$(PWD)/../new-file-server-mona
export MONADIR=$(PWD)/../mona
endif

all:
### basic libraries
### servers
#mumurik	cd Net/uip && $(MAKE) && $(MAKE) install
### libraries
	cd Net/curl && touch dependencies && $(MAKE) && $(MAKE) install
#	cd Graphics/Mesa && touch dependencies && $(MAKE) && $(MAKE) instal
	cd Graphics/pixman && touch dependencies && $(MAKE) && $(MAKE) install
	cd Graphics/cairo && touch dependencies && $(MAKE) && $(MAKE) install
	cd Gui/libgui && touch dependencies && $(MAKE) && $(MAKE) install
	cd Gui/libmonagui && touch dependencies && $(MAKE) && $(MAKE) install
	cd Gui/ime_server && touch dependencies && $(MAKE) && $(MAKE) install
	cd Gui/test_monagui && touch dependencies && $(MAKE) && $(MAKE) install

### non-gui applications
	cd Emulators/infones && touch dependencies && $(MAKE)  && $(MAKE) install
	cd Emulators/monamona && touch dependencies && $(MAKE) && $(MAKE) install
	cd Graphics/jpegview && touch dependencies && $(MAKE) #&& $(MAKE) install
#	cd Graphics/Mesa-gears && touch dependencies && $(MAKE)  && $(MAKE) install
#	cd Graphics/MesaDemos && touch dependencies && $(MAKE) && $(MAKE) install
	cd Misc/helloworld && touch dependencies && $(MAKE) && $(MAKE) install
	cd Misc/print && touch dependencies && $(MAKE) # && $(MAKE) install
	cd Multimedia/audio_server && touch dependencies && $(MAKE) && $(MAKE) install
	cd Multimedia/audio_server/api && touch dependencies && $(MAKE) && $(MAKE) install
	cd Multimedia/audio_server/client && touch dependencies && $(MAKE) && $(MAKE) install
	cd Multimedia/audio_server/happy && touch dependencies && $(MAKE) && $(MAKE) install
	cd Multimedia/monapeg && touch dependencies && $(MAKE) # && $(MAKE) install
	cd Multimedia/monappt && touch dependencies && $(MAKE) # && $(MAKE) install
	cd Multimedia/bplayer && touch dependencies && $(MAKE) # && $(MAKE) install
### MonaForms applications
	cd Shells/mshell && touch dependencies && $(MAKE) && $(MAKE) install
	cd Gui/monafile && touch dependencies && $(MAKE) && $(MAKE) install
	cd Net/monadat && touch dependencies && $(MAKE) && $(MAKE) install
	cd Net/frecv && touch dependencies && $(MAKE) && $(MAKE) install
	cd Net/libxml2 && touch dependencies && $(MAKE) && $(MAKE) install

### Monagui applications
	cd Shells/gshell && touch dependencies && $(MAKE) #&& $(MAKE) install
	cd Shells/terminal && touch dependencies && $(MAKE) && $(MAKE) install
	cd Gui/glaunch && touch dependencies && $(MAKE) && $(MAKE) install
	cd Misc/gnote && touch dependencies && $(MAKE) && $(MAKE) install
	cd Misc/mg-20090107 && touch dependencies && $(MAKE) && $(MAKE) install
	cd Misc/facebook && touch dependencies && $(MAKE) && $(MAKE) install
	cd Multimedia/xmonapeg && touch dependencies && $(MAKE) # && $(MAKE) install
	cd Emulators/monasq && touch dependencies && $(MAKE) && $(MAKE) install
	cd Net/w3m && touch dependencies && $(MAKE) && $(MAKE) install
### Network applications
#	cd Net/libnet/sample/giko && $(MAKE) && $(MAKE) install
### Append
	cd Benchmarks/gfire && $(MAKE) && $(MAKE) install
	cd Benchmarks/gnoiz2bg && $(MAKE) && $(MAKE) install
	cd Games/ghebtn && $(MAKE) && $(MAKE) install
	cd Games/gnullpo && $(MAKE) && $(MAKE) install
	cd Games/gtetris && $(MAKE) &&  $(MAKE) install
	cd Games/mblupin && $(MAKE) && $(MAKE) install
	cd Games/mine && $(MAKE) && $(MAKE) install
	cd Games/mtorz && $(MAKE) && $(MAKE) install
	cd Graphics/bitmap && $(MAKE) && $(MAKE) install
	cd Sysutils/gclock && $(MAKE) && $(MAKE) install
	cd Sysutils/gtimer && $(MAKE) && $(MAKE) install
	cd Sysutils/startx && $(MAKE) && $(MAKE) install
# can't build on gcc 4.x
#	cd Devel/gcc && $(MAKE) # && $(MAKE) install
### $(MAKE) disk images
	cd $(MONADIR)/tool/mkimg && $(MAKE)
	cd Gui/libmonagui/test_on_linux && $(MAKE)

clean:
	cd Net/frecv && $(MAKE) clean
	cd Emulators/infones && $(MAKE) clean
	cd Emulators/monasq && $(MAKE) clean
	cd Emulators/monamona && $(MAKE) clean
	cd Graphics/jpegview && $(MAKE) clean
	cd Gui/libmonagui && $(MAKE) clean
	cd Gui/libgui && $(MAKE) clean
	cd Gui/ime_server && $(MAKE) clean
	cd Graphics/Mesa && $(MAKE) clean
	cd Graphics/cairo && $(MAKE) clean
	cd Graphics/pixman && $(MAKE) clean
	cd Graphics/Mesa-gears && $(MAKE) clean
	cd Graphics/MesaDemos && $(MAKE) clean
	cd Misc/helloworld && $(MAKE) clean
	cd Misc/print && $(MAKE) clean
	cd Misc/gnote && $(MAKE) clean
	cd Misc/mg-20090107 && $(MAKE) clean
	cd Misc/facebook && $(MAKE) clean
	cd Multimedia/audio_server && $(MAKE) clean
	cd Multimedia/bplayer && $(MAKE) clean
	cd Multimedia/audio_server/client && $(MAKE) clean
	cd Multimedia/audio_server/api && $(MAKE) clean
	cd Multimedia/audio_server/happy && $(MAKE) clean
	cd Multimedia/monapeg && $(MAKE) clean
	cd Multimedia/monappt && $(MAKE) clean
	cd Multimedia/xmonapeg && $(MAKE) clean
#	cd Net/libnet && $(MAKE) clean
	cd Net/monadat && $(MAKE) clean
	cd Net/uip && $(MAKE) clean
	cd Shells/gshell && $(MAKE) clean
	cd Shells/mshell && $(MAKE) clean
	cd Shells/terminal && $(MAKE) clean
	cd Gui/glaunch && $(MAKE) clean
	cd Gui/monafile && $(MAKE) clean
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
	cd Gui/test_monagui && $(MAKE) clean
	cd Net/w3m && $(MAKE) clean
	cd Net/curl && $(MAKE) clean
	cd Net/libxml2 && $(MAKE) clean
	cd Devel/gcc && $(MAKE) clean

TARGETS = \
Benchmarks/gfire \
Benchmarks/gnoiz2bg \
Emulators/infones \
Emulators/monamona \
Emulators/monasq \
Graphics/Mesa \
Graphics/Mesa-gears \
Graphics/MesaDemos \
Graphics/jpegview \
Gui/glaunch \
Gui/ime_server \
Gui/libbaygui \
Gui/libgui \
Gui/libmonagui \
Gui/monafile \
Gui/test_baygui \
Gui/test_monagui \
Misc/display \
Misc/facebook/ \
Misc/gnote/ \
Misc/helloworld \
Misc/mg-20090107/ \
Misc/print \
Misc/reverse \
Misc/upper \
Multimedia/audio_server \
Multimedia/audio_server/api \
Multimedia/audio_server/happy \
Multimedia/bplayer \
Multimedia/monapeg \
Multimedia/monappt \
Multimedia/xmonapeg \
Net/frecv \
Net/w3m \
Net/curl \
Net/libxml2 \
Net/monadat \
Shells/gshell \
Shells/mshell/ \
Shells/terminal

depend:
	for i in `echo $(TARGETS)`; do (cd $$i && $(MAKE) $@) ;done
