include Makefile.inc

all : LIB_USER HELLO CLOCK JPEGDEMO KEY MAP MOUSE NOIZ2BG REVERSI SHELLSVR

LIB_USER :
	cd userlib; make

HELLO :
	cd HelloWorld; make

CLOCK :
	cd clock; make

JPEGDEMO :
	cd jpegls; make

KEY :
	cd KeyBoardServer; make

MAP :
	cd MapServer; make

MOUSE :
	cd MouseServer; make

NOIZ2BG :
	cd noiz2bg; make

REVERSI :
	cd Reversi; make

SHELLSVR :
	cd ShellServer; make

clean :
	(cd userlib; make clean)
	(cd HelloWorld; make clean)
	(cd clock; make clean)
	(cd jpegls; make clean)
	(cd KeyBoardServer; make clean)
	(cd MapServer; make clean)
	(cd MouseServer; make clean)
	(cd noiz2bg; make clean)
	(cd Reversi; make clean)
	(cd ShellServer; make clean)

