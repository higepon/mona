#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "elf.h"

#define FILE_BUF (8 * 1024)


int main(int argc, char *argv[]) {

    FILE* fin;
    byte buf[FILE_BUF];

    if ((fin = fopen(argv[1], "rb")) == NULL) {
        printf("can't open %s \n", argv[0]);
        exit (2);
    }

    memset(buf, 0, sizeof(buf));
    fgets((char*)buf, FILE_BUF - 1, fin);

    ELFHeader elf;
    memcpy(&elf, buf, sizeof(elf));

    printf("magic[0]=%x\n", elf.magic[0]);
    printf("magic[1]=%c\n", elf.magic[1]);
    printf("magic[2]=%c\n", elf.magic[2]);
    printf("magic[3]=%c\n", elf.magic[3]);
    printf("clazz           =%x\n", elf.clazz           );
    printf("byteorder       =%x\n", elf.byteorder       );
    printf("headerversion   =%x\n", elf.headerversion   );
    printf("type            =%x\n", elf.type            );
    printf("archtype        =%x\n", elf.archtype        );
    printf("fileversion     =%x\n", elf.fileversion     );
    printf("entrypoint      =%x\n", elf.entrypoint      );
    printf("phdrpos         =%x\n", elf.phdrpos         );
    printf("shdrpos         =%x\n", elf.shdrpos         );
    printf("flag            =%x\n", elf.flag            );
    printf("hdrsize         =%x\n", elf.hdrsize         );
    printf("phdrent         =%x\n", elf.phdrent         );
    printf("phdrcnt         =%x\n", elf.phdrcnt         );
    printf("shdrent         =%x\n", elf.shdrent         );
    printf("shdrcnt         =%x\n", elf.shdrcnt         );
    printf("strsec          =%x\n", elf.strsec          );
}
