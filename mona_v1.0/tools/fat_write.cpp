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

    char filename[128];

    /* arg check */
    if (argc != 3) {
        printf("usage: fat_write.exe fdimage name.ext\n");
        exit(-1);
    }

    /* tokenize */
    char* name = strtok(argv[2], ".");
    char* ext  = strtok(NULL   , ".");
    if (!name || !ext) {
        printf("toknize error %s %s\n", name, ext);
        exit(-1);
    }
    sprintf(filename, "%s.%s", name, ext);

    /* open drivers */
    DiskDriver* driver = new HogeDriver(argv[1], 50);
    FAT12*      fat    = new FAT12(driver);

    /* fat ititilze */
    if (!fat->initilize()) {

        int errorNo = fat->getErrorNo();

        if (errorNo == FAT12::BPB_ERROR) printf("BPB read  error \n");
        else if (errorNo == FAT12::NOT_FAT12_ERROR) printf("NOT FAT12 error \n");
        else if (errorNo == FAT12::FAT_READ_ERROR) printf("FAT read  error \n");
        else printf("unknown error \n");
        delete driver;
        delete fat;
        exit(-1);
    }

    /* create file to write to image */
    if (!fat->createFlie(name, ext)) {

        printf("can not create file %s.%s errorno=%d", name, ext, fat->getErrorNo());
        delete driver;
        delete fat;
        exit(-1);
    }

    /* open file */
    if (!fat->open(".", filename, FAT12::WRITE_MODE)) {

        printf("open %s failed", filename);
        delete driver;
        delete fat;
        exit(-1);
    }

    /* read target file from your PC */
    std::fstream target;
    target.open(filename, std::ios::out|std::ios::in|std::ios::binary);
    if (!target) {
        printf("%s open error\n", filename);
        delete driver;
        delete fat;
        exit(-1);
    }

    /* write file into image */
    byte text[512];
    target.seekg(0);
    while (true) {

        if (!target.read((char*)text, 512)) break;

        if (!fat->write(text)) {
            printf("write failed");
            target.close();
            delete driver;
            delete fat;
            exit(-1);
        }
        memset(text, 0, 512);
    }

    /* close image */
    if (!fat->close()) {
        printf("close failed");
        target.close();
        delete driver;
        delete fat;
        exit(-1);
    }

    /* normal end */
    target.close();
    delete fat;
    delete driver;
    exit(0);
}

void info(int level, const char *format, ...) {

    /* donothing */
    return;
}
