HIGE_TOOLS = ${HIGE_HOME}/tools
NASM       = ${HIGE_TOOLS}/nasm/nasmw.exe
NDISASM    = ${HIGE_TOOLS}/nasm/ndisasmw.exe
CC         = ${HIGE_TOOLS}/djgpp/bin/gcc.exe
LD         = ${HIGE_TOOLS}/djgpp/bin/ld.exe
STRIP      = ${HIGE_TOOLS}/djgpp/bin/strip.exe
DOXYGEN    = doxygen.exe
INCLUDE    = include
LFLAGS     = -Ttext 0x200 --oformat binary
CFLAGS     = -nostdlib -fno-builtin -fno-exceptions -fno-rtti -ffreestanding -Wall -c
BIN        = ../../bin
LIST       = ../../list
SCRIPTS    = ${HIGE_TOOLS}/scripts
FIRST      = ${BIN}/firstboot.bin
SECOND     = ${BIN}/secondboot.bin
THIRD      = ${BIN}/third.bin
SRC        = higepos_kernel.cpp   \
             vga.cpp              \
             idt.cpp              \
             ihandler.cpp         \
             io.cpp               \
             MemoryManager.cpp    \
             x86MemoryManager.cpp \
             purevirtual.cpp      \
             higeOperator.cpp     \
             higeUtil.cpp         \
             Sub.cpp              \
             Base.cpp             \
             Vector.cpp

HEADER     = ${INCLUDE}/kernel.h           \
             ${INCLUDE}/vga.h              \
             ${INCLUDE}/idt.h              \
             ${INCLUDE}/io.h               \
             ${INCLUDE}/Object.h           \
             ${INCLUDE}/MemoryManager.h    \
             ${INCLUDE}/X86MemoryManager.h \
             ${INCLUDE}/higeOperator.h     \
             ${INCLUDE}/higeUtil.h         \
             ${INCLUDE}/Base.h             \
             ${INCLUDE}/Sub.h              \
             ${INCLUDE}/Vector.h
OBJ        = ${BIN}/cstart.o           \
             ${BIN}/higepos_kernel.o   \
             ${BIN}/vga.o              \
             ${BIN}/idt.o              \
             ${BIN}/ihandler.o         \
             ${BIN}/io.o               \
             ${BIN}/MemoryManager.o    \
             ${BIN}/X86MemoryManager.o \
             ${BIN}/higeOperator.o     \
             ${BIN}/purevirtual.o      \
             ${BIN}/higeUtil.o         \
             ${BIN}/Base.o             \
             ${BIN}/Sub.o              \
             ${BIN}/Vector.o


# src to html & document
WEB : ${BIN}/higeboot.bin
	${DOXYGEN}
	cd ${SCRIPTS}; ${SCRIPTS}/web.rb

${BIN}/higeboot.bin : ${FIRST} ${SECOND} ${THIRD}
	cat  ${FIRST} ${SECOND} ${THIRD} >$@

${FIRST} : firstboot.asm boot.mac
	${NASM} firstboot.asm  -o $@ -l ${LIST}/first.list

${SECOND} : secondboot.asm
	${NASM} secondboot.asm  -o $@ -l ${LIST}/second.list

${THIRD} : ${SRC} ${HEADER}
	${CC} -I $(INCLUDE) ${CFLAGS} cstart.cpp -o ${BIN}/cstart.o
	${CC} -I $(INCLUDE) ${CFLAGS} higepos_kernel.cpp -o ${BIN}/higepos_kernel.o
	${CC} -I $(INCLUDE) ${CFLAGS} vga.cpp -o ${BIN}/vga.o
	${CC} -I $(INCLUDE) ${CFLAGS} idt.cpp -o ${BIN}/idt.o
	${CC} -I $(INCLUDE) ${CFLAGS} ihandler.cpp -o ${BIN}/ihandler.o
	${CC} -I $(INCLUDE) ${CFLAGS} io.cpp -o ${BIN}/io.o
	${CC} -I $(INCLUDE) ${CFLAGS} MemoryManager.cpp -o ${BIN}/MemoryManager.o
	${CC} -I $(INCLUDE) ${CFLAGS} X86MemoryManager.cpp -o ${BIN}/X86MemoryManager.o
	${CC} -I $(INCLUDE) ${CFLAGS} purevirtual.cpp -o ${BIN}/purevirtual.o
	${CC} -I $(INCLUDE) ${CFLAGS} higeOperator.cpp -o ${BIN}/higeOperator.o
	${CC} -I $(INCLUDE) ${CFLAGS} higeUtil.cpp -o ${BIN}/higeUtil.o
	${CC} -I $(INCLUDE) ${CFLAGS} Base.cpp -o ${BIN}/Base.o
	${CC} -I $(INCLUDE) ${CFLAGS} Sub.cpp  -o ${BIN}/Sub.o
	${CC} -I $(INCLUDE) ${CFLAGS} Vector.cpp  -o ${BIN}/Vector.o
	${LD} -n -Ttext 0x1200 -static --oformat binary -o${THIRD} ${OBJ}
	${NDISASM} -u $@ > ${LIST}/third_dis.asm
#	${STRIP} --output-target=binary ${OBJ} -o ${THIRD}

clean :
	cd ${BIN}; rm *
