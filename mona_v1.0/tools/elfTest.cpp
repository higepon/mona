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
    fclose(fin);

    ELFHeader* header = (ELFHeader*)buf;

    printf("magic[0]=%x\n", header->magic[0]);
    printf("magic[1]=%c\n", header->magic[1]);
    printf("magic[2]=%c\n", header->magic[2]);
    printf("magic[3]=%c\n", header->magic[3]);
    printf("clazz           =%x\n", header->clazz           );
    printf("byteorder       =%x\n", header->byteorder       );
    printf("headerversion   =%x\n", header->headerversion   );
    printf("type            =%x\n", header->type            );
    printf("archtype        =%x\n", header->archtype        );
    printf("fileversion     =%x\n", header->fileversion     );
    printf("entrypoint      =%x\n", header->entrypoint      );
    printf("phdrpos         =%d\n", header->phdrpos         );
    printf("shdrpos         =%d\n", header->shdrpos         );
    printf("flag            =%x\n", header->flag            );
    printf("hdrsize         =%d\n", header->hdrsize         );
    printf("phdrent         =%d\n", header->phdrent         );
    printf("phdrcnt         =%d\n", header->phdrcnt         );
    printf("shdrent         =%d\n", header->shdrent         );
    printf("shdrcnt         =%d\n", header->shdrcnt         );
    printf("strsec          =%x\n", header->strsec          );

    for (int i = 0; i < header->shdrcnt; i++) {

        ELFSectionHeader* sheader = (ELFSectionHeader*)((dword)header + header->shdrpos + i * (header->shdrent));

        printf("name=%d type=%d flags=%d address=%x offset=%x size=%x link=%d info=%d align=%d entsize=%d\n"
               , sheader->name
               , sheader->type
               , sheader->flags
               , sheader->address
               , sheader->offset
               , sheader->size
               , sheader->link
               , sheader->info
               , sheader->align
               , sheader->entsize);

    }

    for (int j = 0; j < header->phdrcnt; j++) {

        ELFProgramHeader* pheader = (ELFProgramHeader*)((dword)header + header->phdrpos + j * (header->phdrent));

        printf("type=%d offset=%x virtualaddr=%x physaddr=%x filesize=%x memorysize=%x flags=%d align=%x\n"
               , pheader->type
               , pheader->offset
               , pheader->virtualaddr
               , pheader->physaddr
               , pheader->filesize
               , pheader->memorysize
               , pheader->flags
               , pheader->align
               );


    }
}
