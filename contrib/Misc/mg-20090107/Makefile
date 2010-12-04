ifndef $(MONADIR)
export MONADIR=$(PWD)/../../../mona
endif

TARGET  = MG
SOURCES = mona_term.cpp
CSOURCES = \
autoexec.c  dired.c    fileio.c  macro.c      re_search.c  theo.c     word.c \
basename.c  dirname.c  funmap.c  mail.c       region.c     tty.c      yank.c \
basic.c     display.c  grep.c    main.c       search.c     ttyio.c \
buffer.c    echo.c     help.c    match.c      spawn.c      ttykbd.c \
cinfo.c     extend.c   kbd.c     modes.c      strlcat.c    undo.c \
cmode.c     fgetln.c   keymap.c  paragraph.c  strlcpy.c    version.c \
dir.c       file.c     line.c    random.c     strtonum.c   window.c

INSTDIR = $(BINDIR)/APPS/BAYGUI
SHAREDIR   = $(MONADIR)/share
ADDLINK    = -lbaygui-imp -lmonalibc-imp --enable-auto-import
ADDLINKDEP = $(MONADIR)/lib/libbaygui-imp.a $(MONADIR)/lib/libmonalibc-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc

-include dependencies
