
MONADIR	= ../..

AS		=	nasmw
CC		=	gcc
RM		=	rm -f

OBJ		=	./obj

COPT	=	-c -Wall -pipe -O3 -Wpointer-arith -fno-exceptions -fno-rtti \
			-fomit-frame-pointer -fsigned-char

OBJS	=	$(OBJ)/main.o		$(OBJ)/monaelf.o
BINS	=	$(MONADIR)/bin/monaelf


all: $(OBJ) $(BINS)


$(OBJ):
	mkdir $(OBJ)


$(BINS): $(OBJS)
	$(CC) $(OBJS) -o $@


$(OBJ)/main.o:		./main.c
	$(CC) $(COPT) -o $@ $<

$(OBJ)/monaelf.o:	./monaelf.c
	$(CC) $(COPT) -o $@ $<


clean:
	$(RM) $(OBJS)
	$(RM) $(BINS)

