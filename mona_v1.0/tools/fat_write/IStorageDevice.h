
#ifndef _MONA_ISTORAGE_DEVICE_
#define _MONA_ISTORAGE_DEVICE_

#include <stdio.h>

class IStorageDevice
{
 public:
    virtual int open()  = 0;
    virtual int close() = 0;
    virtual int read(int lba, void* buf, int size)  = 0;
    virtual int write(int lba, void* buf, int size) = 0;
};

#endif
