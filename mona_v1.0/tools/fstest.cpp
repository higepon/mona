#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "HogeDriver.h"
#include "FAT12.h"


int main(int argc, char *argv[]) {

    DiskDriver* driver = new HogeDriver("./floppy.dat", 50);

    byte buf[512];
    memset(buf, 0, 512);

    driver->read(0, buf);

    for (int i = 0; i < 512; i++) {
        printf("%x", (byte)(buf[i]));
    }

    FAT12* fat = new FAT12(driver);

    delete driver;
    printf("test sizeof BPB=%d. should be 59", sizeof(BPB));

    return 0;

}
