ifdef CYGWIN
    include cygwin.env
else
    ifdef DJGPP
        include djgpp.env
    else
        include linux.env
    endif
endif
DOXYGEN    = doxygen
BIN        = ../bin
LIST       = ../list
FIRST      = ${BIN}/firstboot.bin
SECOND     = ${BIN}/secondboot.bin
THIRD      = ${BIN}/third.bin
LIBINCLUDE = lib/include
LIBSRC     = lib/string.c
LIBHEADER  = lib/include/string.h
INCLUDE    = include
STLINCLUDE = stlport
CFINCLUDE  = -I${INCLUDE} -I${LIBINCLUDE} -I${STLINCLUDE}
SRC        = monaKernel.cpp       \
             monaVga.cpp          \
             monaIdt.cpp          \
             monaIhandler.cpp     \
             monaIo.cpp           \
             IA32MemoryManager.cpp \
             monaPureVirtual.cpp  \
             monaOperator.cpp     \
             monaUtil.cpp         \
             KeyBoardManager.cpp  \
             FDCDriver.cpp        \
             monaTester.cpp       \
             ProcessManager.cpp   \
             monaChecker.cpp      \
             SystemInfo.cpp       \

HEADER     = ${INCLUDE}/monaKernel.h       \
             ${INCLUDE}/monaVga.h          \
             ${INCLUDE}/monaIdt.h          \
             ${INCLUDE}/monaIo.h           \
             ${INCLUDE}/IA32MemoryManager.h \
             ${INCLUDE}/monaOperator.h     \
             ${INCLUDE}/monaUtil.h         \
             ${INCLUDE}/monaTypes.h        \
             ${INCLUDE}/KeyBoardManager.h  \
             ${INCLUDE}/FDCDriver.h        \
             ${INCLUDE}/monaTester.h       \
             ${INCLUDE}/ProcessManager.h   \
             ${INCLUDE}/monaChecker.h      \
             ${INCLUDE}/SystemInfo.h       \

OBJ        = ${BIN}/cstart.o           \
             ${BIN}/monaKernel.o       \
             ${BIN}/monaVga.o          \
             ${BIN}/monaIdt.o          \
             ${BIN}/monaIhandler.o     \
             ${BIN}/monaIo.o           \
             ${BIN}/IA32MemoryManager.o \
             ${BIN}/monaOperator.o     \
             ${BIN}/monaPureVirtual.o  \
             ${BIN}/monaUtil.o         \
             ${BIN}/KeyBoardManager.o  \
             ${BIN}/FDCDriver.o        \
             ${BIN}/monaTester.o       \
             ${BIN}/ProcessManager.o   \
             ${BIN}/monaChecker.o      \
             ${BIN}/string.o           \
             ${BIN}/SystemInfo.o       \
             ${BIN}/memcmp.o           \
             ${BIN}/memmove.o          \

# document
WEB : ${BIN}/mona.img
	${DOXYGEN}

${BIN}/mona.img : ${FIRST} ${SECOND} ${THIRD}
	cat  ${FIRST} ${SECOND} ${THIRD} >$@
	cat ${BIN}/mona.img /dev/zero|dd of=${BIN}/Mona.vfd bs=1k count=1440

${FIRST} : firstboot.asm boot.mac
	${NASM} firstboot.asm  -o $@ -l ${LIST}/first.list

${SECOND} : secondboot.asm
	${NASM} secondboot.asm  -o $@ -l ${LIST}/second.list

${THIRD} : ${SRC} ${HEADER} ${LIBSRC} ${LIBHEADER}
	${CC} ${CFINCLUDE} ${CFLAGS} cstart.cpp           -o ${BIN}/cstart.o
	${CC} ${CFINCLUDE} ${CFLAGS} monaKernel.cpp       -o ${BIN}/monaKernel.o
	${CC} ${CFINCLUDE} ${CFLAGS} monaVga.cpp          -o ${BIN}/monaVga.o
	${CC} ${CFINCLUDE} ${CFLAGS} monaIdt.cpp          -o ${BIN}/monaIdt.o
	${CC} ${CFINCLUDE} ${CFLAGS} monaIhandler.cpp     -o ${BIN}/monaIhandler.o
	${CC} ${CFINCLUDE} ${CFLAGS} monaIo.cpp           -o ${BIN}/monaIo.o
	${CC} ${CFINCLUDE} ${CFLAGS} IA32MemoryManager.cpp -o ${BIN}/IA32MemoryManager.o
	${CC} ${CFINCLUDE} ${CFLAGS} monaPureVirtual.cpp  -o ${BIN}/monaPureVirtual.o
	${CC} ${CFINCLUDE} ${CFLAGS} monaOperator.cpp     -o ${BIN}/monaOperator.o
	${CC} ${CFINCLUDE} ${CFLAGS} monaUtil.cpp         -o ${BIN}/monaUtil.o
	${CC} ${CFINCLUDE} ${CFLAGS} KeyBoardManager.cpp  -o ${BIN}/KeyBoardManager.o
	${CC} ${CFINCLUDE} ${CFLAGS} FDCDriver.cpp        -o ${BIN}/FDCDriver.o
	${CC} ${CFINCLUDE} ${CFLAGS} monaTester.cpp       -o ${BIN}/monaTester.o
	${CC} ${CFINCLUDE} ${CFLAGS} ProcessManager.cpp   -o ${BIN}/ProcessManager.o
	${CC} ${CFINCLUDE} ${CFLAGS} monaChecker.cpp      -o ${BIN}/monaChecker.o
	${CC} ${CFINCLUDE} ${CFLAGS} ./lib/string.c       -o ${BIN}/string.o
	${CC} ${CFINCLUDE} ${CFLAGS} SystemInfo.cpp       -o ${BIN}/SystemInfo.o
	${CC} ${CFINCLUDE} ${CFLAGS} ./FreeBSD/memcmp.c   -o ${BIN}/memcmp.o
	${CC} ${CFINCLUDE} ${CFLAGS} ./FreeBSD/memmove.c  -o ${BIN}/memmove.o
	${LD} ${LFLAGS} -o${THIRD} ${OBJ}
	${NDISASM} -u $@ > ${LIST}/third.list
	${STRIP}  -O binary ${THIRD}

clean :
	cd ${BIN}; rm *.bin; rm *.o; rm *.img
