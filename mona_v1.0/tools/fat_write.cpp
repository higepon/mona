/*!
    \file   fat_write.cpp
    \brief  tool for mona os. create FAT12 boot fd image.

    Copyright (c) 2002, 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/05/31 update:$Date$
*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "HogeDriver.h"
#include "FAT12.h"
#include "string.h"

int main(int argc, char *argv[]) {

//     if (1) {
//         printf("usage: fat_write.exe image_file file_to_write\n");
//         exit(-1);
//     }


//     exit(0);

    printf("making %s...\n", argv[1]);

    DiskDriver* driver = new HogeDriver(argv[1], 50);
    FAT12*      fat    = new FAT12(driver);
    std::fstream target;
    std::fstream target2;

    target.open("../bin/KERNEL.IMG", std::ios::out|std::ios::in|std::ios::binary);

    if (!fat->initilize()) {

        int errorNo = fat->getErrorNo();

        if (errorNo == FAT12::BPB_ERROR) printf("BPB read  error \n");
        else if (errorNo == FAT12::NOT_FAT12_ERROR) printf("NOT FAT12 error \n");
        else if (errorNo == FAT12::FAT_READ_ERROR) printf("NOT FAT12 error \n");
        else printf("unknown error \n");

        target.close();
        return -1;
    }

    printf("fat initilize OK\n");

    printf("create file start \n");
    if (!fat->createFlie("KERNEL", "IMG")) {

        printf("can not create file=%d", fat->getErrorNo());
        return -1;
    }

    printf("open file start\n");
    if (!fat->open(".", "KERNEL.IMG", FAT12::WRITE_MODE)) {

        printf("open failed");
        return -1;
    }

    byte text[512];
    target.seekg(0);
    while (true) {

        if (!target.read((char*)text, 512)) break;

        if (!fat->write(text)) {
            printf("write failed");
            target.close();
            return -1;
        }
        memset(text, 0, 512);
    }

    if (!fat->close()) {
        printf("close failed");
        target.close();
    }

    target.close();

    delete fat;

   fat    = new FAT12(driver);

   target2.open("../bin/USER.ELF", std::ios::out|std::ios::in|std::ios::binary);

   if (!fat->initilize()) {

       int errorNo = fat->getErrorNo();

       if (errorNo == FAT12::BPB_ERROR) printf("BPB read  error \n");
       else if (errorNo == FAT12::NOT_FAT12_ERROR) printf("NOT FAT12 error \n");
       else if (errorNo == FAT12::FAT_READ_ERROR) printf("NOT FAT12 error \n");
       else printf("unknown error \n");

       target2.close();
       return -1;
   }

   printf("fat initilize OK\n");

   printf("create file start \n");
   if (!fat->createFlie("USER", "ELF")) {

       printf("can not create file=%d", fat->getErrorNo());
       return -1;
   }

   printf("open file start\n");
   if (!fat->open(".", "USER.ELF", FAT12::WRITE_MODE)) {

       printf("open failed");
       return -1;
   }

   target2.seekg(0);
   while (true) {

       if (!target2.read((char*)text, 512)) break;

       if (!fat->write(text)) {
           printf("write failed");
           target2.close();
           return -1;
       }
       memset(text, 0, 512);
   }

   if (!fat->close()) {
       printf("close failed");
       target2.close();
   }

   target2.close();

   delete fat;
    delete driver;

    return 0;
}

void info(int level, const char *format, ...) {

    /* donothing */
    return;
}
