MONADIR = ../..
include $(MONADIR)/env/Makefile.inc

RM		=	rm -f
OBJ		=	./obj

COPT	=	-c -Wall -pipe -O3 -Wpointer-arith -fno-exceptions -fno-rtti \
			-fomit-frame-pointer -fsigned-char

OBJS	=	$(OBJ)/main.o     $(OBJ)/monaelf.o
BINS	=	$(MONADIR)/bin/monaelf$(EXEEXT)


all: $(OBJ) $(BINS)

$(OBJ):
	mkdir $(OBJ)

$(BINS): $(OBJS)
	$(CXX) $(OBJS) -o $@


$(OBJ)/main.o:		./main.c ./monaelf.h
	$(CXX) $(COPT) -o $@ $<

$(OBJ)/monaelf.o:	./monaelf.c ./elfhdr.h ./monaelf.h
	$(CXX) $(COPT) -o $@ $<


clean:
	$(RM) $(OBJS)
	$(RM) $(BINS)

