ifdef CYGWIN
    include cygwin.env
else
    ifdef DJGPP
        include djgpp.env
    else
        include linux.env
    endif
endif
BIN        = ../bin
LIST       = ../list
FIRST      = ${BIN}/firstboot.bin
SECOND     = ${BIN}/secondboot.bin
THIRD      = ${BIN}/third.bin
SRC        = monaKernel.cpp       \
             monaVga.cpp          \
             monaIdt.cpp          \
             monaIhandler.cpp     \
             monaIo.cpp           \
             X86MemoryManager.cpp \
             monaPureVirtual.cpp  \
             monaOperator.cpp     \
             monaUtil.cpp         \
             KeyBoardManager.cpp  \
             FDCDriver.cpp        \
             monaTester.cpp

HEADER     = ${INCLUDE}/monaKernel.h       \
             ${INCLUDE}/monaVga.h          \
             ${INCLUDE}/monaIdt.h          \
             ${INCLUDE}/monaIo.h           \
             ${INCLUDE}/X86MemoryManager.h \
             ${INCLUDE}/monaOperator.h     \
             ${INCLUDE}/monaUtil.h         \
             ${INCLUDE}/monaTypes.h        \
             ${INCLUDE}/KeyBoardManager.h  \
             ${INCLUDE}/FDCDriver.h        \
             ${INCLUDE}/monaTester.h

OBJ        = ${BIN}/cstart.o           \
             ${BIN}/monaKernel.o       \
             ${BIN}/monaVga.o          \
             ${BIN}/monaIdt.o          \
             ${BIN}/monaIhandler.o     \
             ${BIN}/monaIo.o           \
             ${BIN}/X86MemoryManager.o \
             ${BIN}/monaOperator.o     \
             ${BIN}/monaPureVirtual.o  \
             ${BIN}/monaUtil.o         \
             ${BIN}/KeyBoardManager.o  \
             ${BIN}/FDCDriver.o        \
             ${BIN}/monaTester.o       \

# src to html & document
WEB : ${BIN}/mona.img
	${DOXYGEN}

${BIN}/mona.img : ${FIRST} ${SECOND} ${THIRD}
	cat  ${FIRST} ${SECOND} ${THIRD} >$@

${FIRST} : firstboot.asm boot.mac
	${NASM} firstboot.asm  -o $@ -l ${LIST}/first.list

${SECOND} : secondboot.asm
	${NASM} secondboot.asm  -o $@ -l ${LIST}/second.list

${THIRD} : ${SRC} ${HEADER}
	${CC} -I $(INCLUDE) ${CFLAGS} cstart.cpp -o ${BIN}/cstart.o
	${CC} -I $(INCLUDE) ${CFLAGS} monaKernel.cpp -o ${BIN}/monaKernel.o
	${CC} -I $(INCLUDE) ${CFLAGS} monaVga.cpp -o ${BIN}/monaVga.o
	${CC} -I $(INCLUDE) ${CFLAGS} monaIdt.cpp -o ${BIN}/monaIdt.o
	${CC} -I $(INCLUDE) ${CFLAGS} monaIhandler.cpp -o ${BIN}/monaIhandler.o
	${CC} -I $(INCLUDE) ${CFLAGS} monaIo.cpp -o ${BIN}/monaIo.o
	${CC} -I $(INCLUDE) ${CFLAGS} X86MemoryManager.cpp -o ${BIN}/X86MemoryManager.o
	${CC} -I $(INCLUDE) ${CFLAGS} monaPureVirtual.cpp -o ${BIN}/monaPureVirtual.o
	${CC} -I $(INCLUDE) ${CFLAGS} monaOperator.cpp -o ${BIN}/monaOperator.o
	${CC} -I $(INCLUDE) ${CFLAGS} monaUtil.cpp -o ${BIN}/monaUtil.o
	${CC} -I $(INCLUDE) ${CFLAGS} KeyBoardManager.cpp  -o ${BIN}/KeyBoardManager.o
	${CC} -I $(INCLUDE) ${CFLAGS} FDCDriver.cpp  -o ${BIN}/FDCDriver.o
	${CC} -I $(INCLUDE) ${CFLAGS} monaTester.cpp  -o ${BIN}/monaTester.o
	${LD} ${LFLAGS} -o${THIRD} ${OBJ}
	${NDISASM} -u $@ > ${LIST}/third.list
	${STRIP}  -O binary ${THIRD}

clean :
	cd ${BIN}; rm *.bin; rm *.o; rm *.img
