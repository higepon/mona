NASM       = nasmw
NDISASM    = ndisasmw
CC         = ${DJGPP_HOME_H}/bin/gpp
LD         = ${DJGPP_HOME_H}/bin/ld
STRIP      = ${DJGPP_HOME_H}/bin/strip
DOXYGEN    = doxygen
INCLUDE    = include
LFLAGS     = -n -Ttext 0x1200 -static --oformat binary
CFLAGS     = -nostdlib -fno-exceptions -fno-rtti -ffreestanding -fno-builtin -Wall -c
BIN        = ../bin
LIST       = ../list
FIRST      = ${BIN}/firstboot.bin
SECOND     = ${BIN}/secondboot.bin
THIRD      = ${BIN}/third.bin
SRC        = higeKernel.cpp   \
             higeVga.cpp              \
             higeIdt.cpp              \
             higeIhandler.cpp         \
             higeIo.cpp               \
             x86MemoryManager.cpp \
             higePureVirtual.cpp      \
             higeOperator.cpp     \
             higeUtil.cpp         \
             KeyBoardManager.cpp

HEADER     = ${INCLUDE}/higeKernel.h           \
             ${INCLUDE}/higeVga.h              \
             ${INCLUDE}/higeIdt.h              \
             ${INCLUDE}/higeIo.h               \
             ${INCLUDE}/X86MemoryManager.h \
             ${INCLUDE}/higeOperator.h     \
             ${INCLUDE}/higeUtil.h         \
             ${INCLUDE}/higeTypes.h     \
             ${INCLUDE}/KeyBoardManager.h

OBJ        = ${BIN}/cstart.o           \
             ${BIN}/higeKernel.o   \
             ${BIN}/higeVga.o              \
             ${BIN}/higeIdt.o              \
             ${BIN}/higeIhandler.o         \
             ${BIN}/higeIo.o               \
             ${BIN}/X86MemoryManager.o \
             ${BIN}/higeOperator.o     \
             ${BIN}/higePureVirtual.o      \
             ${BIN}/higeUtil.o         \
             ${BIN}/KeyBoardManager.o


# src to html & document
WEB : ${BIN}/higeboot.bin
	${DOXYGEN}

${BIN}/higeboot.bin : ${FIRST} ${SECOND} ${THIRD}
	cat  ${FIRST} ${SECOND} ${THIRD} >$@

${FIRST} : firstboot.asm boot.mac
	${NASM} firstboot.asm  -o $@ -l ${LIST}/first.list

${SECOND} : secondboot.asm
	${NASM} secondboot.asm  -o $@ -l ${LIST}/second.list

${THIRD} : ${SRC} ${HEADER}
	${CC} -I $(INCLUDE) ${CFLAGS} cstart.cpp -o ${BIN}/cstart.o
	${CC} -I $(INCLUDE) ${CFLAGS} higeKernel.cpp -o ${BIN}/higeKernel.o
	${CC} -I $(INCLUDE) ${CFLAGS} higeVga.cpp -o ${BIN}/higeVga.o
	${CC} -I $(INCLUDE) ${CFLAGS} higeIdt.cpp -o ${BIN}/higeIdt.o
	${CC} -I $(INCLUDE) ${CFLAGS} higeIhandler.cpp -o ${BIN}/higeIhandler.o
	${CC} -I $(INCLUDE) ${CFLAGS} higeIo.cpp -o ${BIN}/higeIo.o
	${CC} -I $(INCLUDE) ${CFLAGS} X86MemoryManager.cpp -o ${BIN}/X86MemoryManager.o
	${CC} -I $(INCLUDE) ${CFLAGS} higePureVirtual.cpp -o ${BIN}/higePureVirtual.o
	${CC} -I $(INCLUDE) ${CFLAGS} higeOperator.cpp -o ${BIN}/higeOperator.o
	${CC} -I $(INCLUDE) ${CFLAGS} higeUtil.cpp -o ${BIN}/higeUtil.o
	${CC} -I $(INCLUDE) ${CFLAGS} KeyBoardManager.cpp  -o ${BIN}/KeyBoardManager.o
	${LD} ${LFLAGS} -o${THIRD} ${OBJ}
	${NDISASM} -u $@ > ${LIST}/third.list
#	${STRIP} --output-target=binary ${OBJ} -o ${THIRD}

clean :
	cd ${BIN}; rm *.bin; rm *.o
