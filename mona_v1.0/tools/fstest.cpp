#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "HogeDriver.h"
#include "FAT12.h"

int main(int argc, char *argv[]) {

    DiskDriver* driver = new HogeDriver("./floppy.dat", 50);
    FAT12*      fat    = new FAT12(driver);

    if (!fat->initilize()) {

        int errorNo = fat->getErrorNo();

        if (errorNo == FAT12::BPB_ERROR) printf("BPB read  error \n");
        else if (errorNo == FAT12::NOT_FAT12_ERROR) printf("NOT FAT12 error \n");
        else if (errorNo == FAT12::FAT_READ_ERROR) printf("NOT FAT12 error \n");
        else printf("unknown error \n");

        /* error */
        return -1;
    }

    printf("fat initilize\n");

    if (!fat->open("SOMEDIR\\DIR1\\DIR2\\DIR3\\DIR4", "SOME.CPP", FAT12::READ_MODE)) {
        printf("open failed");
    }

    while (fat->readHasNext()) {
        byte buf[512];
        fat->read(buf);
        for (int i = 0; i < 512; i++) printf("%c", (char)buf[i]);
    }

    return 0;

    if (!fat->changeDirectoryRelative("SOMEDIR")) {
        printf("some dir not found");
    }

    if (!fat->open("..", "HELLO.TXT", FAT12::READ_MODE)) {

        printf("open failed");
    }


    if (!fat->changeDirectory("SOMEDIR\\DIR1\\DIR2\\DIR3\\DIR4")) {
        printf("changeDirectory failed");
        return -1;
    }

    if (!fat->changeDirectoryRelative("SOMEDIR")) {
        printf("some dir not found");
    }

    if (!fat->changeDirectoryRelative(".")) {
        printf(". not found");
    }

    if (!fat->changeDirectoryRelative("DIR1")) {
        printf("dir1 not found");
    }

    if (!fat->changeDirectoryRelative("DIR2")) {
        printf("dir2 not found");
    }

    if (!fat->changeDirectoryRelative("DIR3")) {
        printf("dir3 not found");
    }

    if (!fat->changeDirectoryRelative("DIR4")) {
        printf("dir4 not found");
    }

    if (!fat->changeDirectoryRelative("..")) {
        printf(".. not found");
    }


    delete fat;
    delete driver;

    return 0;
}

