
MONADIR = ../../..
include $(MONADIR)/env/dirnames.inc
include $(MONADIR)/env/Makefile.inc

LFLAGS		= $(MONAELF_LDS) -n -Ttext 0xA0000000 -e $(USER_START_FUNCTION) -Bstatic
OFLAGS		= --output-target=elf32-i386

RM		=	rm -f


#object dir
OBJ		=	./obj
BIN		=	./bin

#source dir
BASE	= .


HEADERS	=	$(BASE)/compiler.h $(BASE)/common.h $(BASE)/syscall.h \
			$(BASE)/libc.h $(BASE)/milstr.h

OBJS	=	$(OBJ)/syscall.o	$(OBJ)/libc.o							\
			$(OBJ)/shell2.o		$(OBJ)/parser.o							\
			$(OBJ)/milstr.o		$(OBJ)/global.o							\
			$(OBJ)/dosio.o		$(OBJ)/textfile.o

BINS	=	$(BIN)/SHELL.SVR

AOPT	=	$(NFLAGS)
COPT	=	-c -pipe -fomit-frame-pointer -fsigned-char \
			$(CXXFLAGS) -I$(MONADIR)/include -I$(BASE) -DNDEBUG

all: $(OBJ) $(BIN) $(BINS)


$(OBJ):
	mkdir $(OBJ)

$(BIN):
	mkdir $(BIN)

$(BINS): $(OBJS)
	$(LD) --Map shell.map $(OBJS) $(LFLAGS) -o $@
	objcopy $(OFLAGS) $@
	$(STRIP) $@
	$(BINDIR)/monaelf $@

install: all
	$(INSTALL) $(BINS) $(BINDIR)/root/SERVERS

clean:
	$(RM) $(OBJS) $(BINS)


$(OBJ)/syscall.o:	$(BASE)/syscall.asm
	$(NASM) $(AOPT) $< -o $@

$(OBJ)/libc.o:		$(BASE)/libc.cpp $(HEADERS)
	$(CXX) $(COPT) -o $@ $<


$(OBJ)/shell2.o:	$(BASE)/shell2.cpp $(HEADERS) $(BASE)/dosio.h \
					$(BASE)/shell2.h $(BASE)/parser.h $(BASE)/global.h
	$(CXX) $(COPT) -o $@ $<

$(OBJ)/parser.o:	$(BASE)/parser.cpp $(HEADERS) $(BASE)/shell2.h \
					$(BASE)/parser.h $(BASE)/dosio.h $(BASE)/textfile.h
	$(CXX) $(COPT) -o $@ $<


$(OBJ)/milstr.o:	$(BASE)/milstr.cpp $(HEADERS)
	$(CXX) $(COPT) -o $@ $<

$(OBJ)/global.o:	$(BASE)/global.cpp $(HEADERS) $(BASE)/global.h
	$(CXX) $(COPT) -o $@ $<

$(OBJ)/dosio.o:		$(BASE)/dosio.cpp $(HEADERS) $(BASE)/dosio.h \
					$(BASE)/shell2.h $(BASE)/global.h
	$(CXX) $(COPT) -o $@ $<

$(OBJ)/textfile.o:	$(BASE)/textfile.cpp $(HEADERS) $(BASE)/dosio.h \
					$(BASE)/textfile.h
	$(CXX) $(COPT) -o $@ $<

