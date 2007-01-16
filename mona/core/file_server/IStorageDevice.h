/*!
    \file  IStorageDevice.h
    \brief interface IStorageDevice

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2004/03/10 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/
#ifndef _MONA_ISTORAGE_DEVICE_
#define _MONA_ISTORAGE_DEVICE_

#include <sys/types.h>

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
