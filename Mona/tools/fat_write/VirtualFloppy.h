#ifndef _VIRTUAL_FLOPPY_
#define _VIRTUAL_FLOPPY__

#include <stdio.h>
#include "IStorageDevice.h"
#ifdef Mona
#include "sys/types.h"
#else
#include "types.h"
#endif


class VirtualFloppy : public IStorageDevice
{
public:
    VirtualFloppy(FILE* fp);
    virtual ~VirtualFloppy();

public:
    int open();
    int close();
    int read(dword lba, void* buf, int size);
    int write(dword lba, void* buf, int size);
    int ioctl(void* p);

private:
    FILE* fp;
};

#endif
