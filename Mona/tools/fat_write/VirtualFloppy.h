#ifndef _VIRTUAL_FLOPPY_
#define _VIRTUAL_FLOPPY__

#include <stdio.h>
#include "IStorageDevice.h"

class VirtualFloppy : public IStorageDevice
{
public:
    VirtualFloppy(FILE* fp);
    virtual ~VirtualFloppy();

public:
    int open();
    int close();
    int read(int lba, void* buf, int size);
    int write(int lba, void* buf, int size);
    int ioctl(void* p);

private:
    FILE* fp;
};

#endif
