#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "DiskDriver.h"

main(int argc, char *argv[]) {

    DiskDriver* driver = new DiskDriver("./floppy.dat", 50);

    unsigned char buf[512];
    memset(buf, 0, 512);

    driver->read((char*)buf, 0);

    for (int i = 0; i < 512; i++) {
	printf("%x", buf[i]);
    }

    delete driver;
    printf("test");

}
