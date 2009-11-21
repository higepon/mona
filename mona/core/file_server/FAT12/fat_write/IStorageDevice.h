/*!
    \file  IStorageDevice.h
    \brief interface IStorageDevice

    Copyright (c) 2004 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2004/03/10 update:$Date$
*/
#ifndef _MONA_ISTORAGE_DEVICE_
#define _MONA_ISTORAGE_DEVICE_

#include "types.h"

class IStorageDevice
{
 public:
    IStorageDevice() {};
    virtual ~IStorageDevice() {};
    virtual int open()  = 0;
    virtual int close() = 0;
    virtual int read(uint32_t lba, void* buf, int size)  = 0;
    virtual int write(uint32_t lba, void* buf, int size) = 0;
    virtual int ioctl(void* p) = 0;
};

#endif
