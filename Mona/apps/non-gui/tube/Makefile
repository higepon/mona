
MONADIR	= ../../..
include $(MONADIR)/env/monapi.inc

RM		=	rm -f

#object dir
OBJ		=	./obj
BIN		=	./bin

OBJS	=	$(OBJ)/tube.oa

BINS	=	$(BIN)/TUBE.ELF

all: $(OBJ) $(BIN) $(BINS)


$(OBJ):
	mkdir $(OBJ)

$(BIN):
	mkdir $(BIN)

$(BINS): $(OBJS)
	$(LD) $(OBJS) $(LFLAGS) -o $@
	objcopy $(OFLAGS) $@
	$(STRIP) --remove-section=.idata $@
	$(STRIP) $@
	$(BINDIR)/monaelf $@

install: all
	mkdir -p $(BINDIR)/root/APPS
	cp -u $(BINS) $(BINDIR)/root/APPS

clean:
	$(RM) $(OBJS) $(BINS)

$(OBJ)/tube.oa:		tube.x86
	$(NASM) $(NFLAGS) $< -o $@

