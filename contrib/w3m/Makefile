#
# Makefile
#

TARGET  = W3M
INSTDIR = $(BINDIR)/APPS/W3M/$(TARGET).APP
DOCDIR=doc-jp
INSTFILES  = $(DOCDIR)/MANUAL.html $(DOCDIR)/FAQ.html  $(DOCDIR)/STORY.html  $(DOCDIR)/README  $(DOCDIR)/README.cygwin  $(DOCDIR)/README.dict  $(DOCDIR)/README.func  $(DOCDIR)/README.img  $(DOCDIR)/README.m17n  $(DOCDIR)/README.mouse  $(DOCDIR)/README.pre_form  $(DOCDIR)/README.tab  $(DOCDIR)/keymap.default  $(DOCDIR)/keymap.lynx  $(DOCDIR)/menu.default  $(DOCDIR)/menu.submenu W3M.map
CLEANFILES = funcname.c funcname1.h funcname2.h
MONADIR=../../mona
SHAREDIR   = $(MONADIR)/share
include Makefile.boehm_gc.inc
AWK=awk


PACKAGE = w3m
VERSION = 0.5.2
top_srcdir = .
DESTDIR = 

MONAAPP_PATH=/APPS/W3M/W3M.APP

CGIBIN_DIR = $(MONAAPP_PATH)
AUXBIN_DIR = $(MONAAPP_PATH)
HELP_DIR = $(MONAAPP_PATH)
RC_DIR =\/MEM
ETC_DIR = $(MONAAPP_PATH)
CONF_DIR = $(MONAAPP_PATH)
LOCALE_DIR = $(MONAAPP_PATH)

WCCFLAGS =
CPPFLAGS = 
DEFS = -DHAVE_CONFIG_H -DAUXBIN_DIR=\"$(AUXBIN_DIR)\" \
	-DCGIBIN_DIR=\"$(CGIBIN_DIR)\" -DHELP_DIR=\"$(HELP_DIR)\" \
	-DETC_DIR=\"$(ETC_DIR)\" -DCONF_DIR=\"$(CONF_DIR)\" \
	-DRC_DIR=\"$(RC_DIR)\" \
        -DLOCALEDIR=\"$(LOCALE_DIR)\" \
	-DUSE_UNICODE=1
LIBS = 
W3M_LIBS = 
WCTARGET = 
NLSTARGET = 


IMGCFLAGS =  
IMGLDFLAGS =  

KEYBIND_OBJ = keybind.o

VERSION=0.5.2+cvs-1.1001
MODEL=-EN

M17NOBJS = libwc/big5.o \
		libwc/ces.o \
		libwc/char_conv.o \
		libwc/charset.o \
		libwc/combining.o \
		libwc/conv.o \
		libwc/detect.o \
		libwc/gb18030.o \
		libwc/gbk.o \
		libwc/hkscs.o \
		libwc/hz.o \
		libwc/iso2022.o \
		libwc/jis.o \
		libwc/johab.o \
		libwc/priv.o \
		libwc/putc.o \
		libwc/search.o \
		libwc/sjis.o \
		libwc/status.o \
		libwc/ucs.o \
		libwc/uhc.o \
		libwc/utf7.o \
		libwc/utf8.o \
		libwc/viet.o \
		libwc/wtf.o


CPPOBJS=main_mona.o terms_mona.o
OBJS=file.o buffer.o display.o etc.o search.o linein.o table.o local.o\
	form.o map.o frame.o rc.o menu.o mailcap.o image.o \
	symbol.o entity.o url.o ftp.o mimehead.o regex.o news.o \
	func.o cookie.o history.o backend.o $(M17NOBJS) $(KEYBIND_OBJ)
LSRCS=anchor.c parsetagx.c tagtable.c istream.c
LOBJS=anchor.o parsetagx.o tagtable.o istream.o
LLOBJS=version.o
ALIBOBJS=Str.o indep.o regex.o textlist.o parsetag.o myctype.o hash.o $(GC_OBJECTS)
ALIB=libindep.a
ALLCOBJS=$(OBJS) $(LOBJS) $(LLOBJS)
ALLOBJS=$(CPPOBJS) $(ALLCOBJS)
ADDTARGETDEP=funcname.c funcname1.h funcname2.h


SOURCES=$(CPPOBJS:.o=.cpp)
CSOURCES=$(ALLCOBJS:.o=.c)
OBJECTS=$(ALLOBJS)

ifneq ($(BUILD_TARGET),ELF)
ADDLINK    =  -L. -lindep -lmonalibc-imp -lbaygui-imp --enable-auto-import
ADDLINKDEP = $(MONADIR)/lib/libbaygui-imp.a $(MONADIR)/lib/libmonalibc-imp.a libindep.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -L. -lindep -lbaygui-imp -lmonalibc-imp -lmonapi-imp
ADDLINKDEP = $(MONADIR)/lib/libbaygui.a $(MONADIR)/lib/libmonalibc.a libindep.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += $(GC_CFLAGS) -g -idirafter -mno-stack-arg-probe -DMONA
CFLAGS   += $(DEFS) $(GC_CFLAGS) -g -idirafter -mno-stack-arg-probe -DMONA -I./ -I./libwc/





$(ALIB): $(ALIBOBJS)
	$(AR) $(ALIB) $(ALIBOBJS)
	$(RANLIB) $(ALIB)

$(OBJS) $(LOBJS): fm.h funcname1.h


indep.o: indep.c fm.h funcname1.h
func.o: funcname.c functable.c funcname1.h
keybind.o: funcname2.h
keybind_lynx.o: funcname2.h
parsetagx.o: html.c


funcname.c: funcname.tab
	sort funcname.tab | $(AWK) -f $(top_srcdir)/funcname0.awk > $@

funcname1.h: funcname.tab
	sort funcname.tab | $(AWK) -f $(top_srcdir)/funcname1.awk > $@

funcname2.h: funcname.tab
	sort funcname.tab | $(AWK) -f $(top_srcdir)/funcname2.awk > $@

 
dummy.o: entity.c
	$(CC) $(CFLAGS) -DDUMMY -c -o $@ $?

#begin GC related files copy rule 

gc_cpp.o: $(BOEHM_GC_DIR)/gc_cpp.cpp
	cp $(BOEHM_GC_DIR)/gc_cpp.o gc_cpp.o

alloc.o: $(BOEHM_GC_DIR)/alloc.c
	cp $(BOEHM_GC_DIR)/alloc.o alloc.o

backgraph.o: $(BOEHM_GC_DIR)/backgraph.c
	cp $(BOEHM_GC_DIR)/backgraph.o backgraph.o

blacklst.o: $(BOEHM_GC_DIR)/blacklst.c
	cp $(BOEHM_GC_DIR)/blacklst.o blacklst.o

dbg_mlc.o: $(BOEHM_GC_DIR)/dbg_mlc.c
	cp $(BOEHM_GC_DIR)/dbg_mlc.o dbg_mlc.o

dyn_load.o: $(BOEHM_GC_DIR)/dyn_load.c
	cp $(BOEHM_GC_DIR)/dyn_load.o dyn_load.o

finalize.o: $(BOEHM_GC_DIR)/finalize.c
	cp $(BOEHM_GC_DIR)/finalize.o finalize.o

headers.o: $(BOEHM_GC_DIR)/headers.c
	cp $(BOEHM_GC_DIR)/headers.o headers.o

mach_dep.o: $(BOEHM_GC_DIR)/mach_dep.c
	cp $(BOEHM_GC_DIR)/mach_dep.o mach_dep.o

malloc.o: $(BOEHM_GC_DIR)/malloc.c
	cp $(BOEHM_GC_DIR)/malloc.o malloc.o

mallocx.o: $(BOEHM_GC_DIR)/mallocx.c
	cp $(BOEHM_GC_DIR)/mallocx.o mallocx.o

mark.o: $(BOEHM_GC_DIR)/mark.c
	cp $(BOEHM_GC_DIR)/mark.o mark.o

mark_rts.o: $(BOEHM_GC_DIR)/mark_rts.c
	cp $(BOEHM_GC_DIR)/mark_rts.o mark_rts.o

misc.o: $(BOEHM_GC_DIR)/misc.c
	cp $(BOEHM_GC_DIR)/misc.o misc.o

new_hblk.o: $(BOEHM_GC_DIR)/new_hblk.c
	cp $(BOEHM_GC_DIR)/new_hblk.o new_hblk.o

obj_map.o: $(BOEHM_GC_DIR)/obj_map.c
	cp $(BOEHM_GC_DIR)/obj_map.o obj_map.o

os_dep.o: $(BOEHM_GC_DIR)/os_dep.c
	cp $(BOEHM_GC_DIR)/os_dep.o os_dep.o

reclaim.o: $(BOEHM_GC_DIR)/reclaim.c
	cp $(BOEHM_GC_DIR)/reclaim.o reclaim.o

allchblk.o: $(BOEHM_GC_DIR)/allchblk.c
	cp $(BOEHM_GC_DIR)/allchblk.o allchblk.o

#end GC related files copy rule


ICONS.BM5: icons.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

alclean:
	rm $(ALIBOBJS)

cppclean:
	rm $(CPPOBJS)


rebuild:
	$(MAKE) cppclean && $(MAKE) all && $(MAKE) install





-include dependencies
