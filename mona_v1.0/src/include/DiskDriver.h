/*!
    \file  DiskDriver.h
    \brief class Disk Driver

    class DiskDriver abstract

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/04/11 update:$Date$
*/
#ifndef _MONA_DISK_DRIVER_
#define _MONA_DISK_DRIVER_

#include<types.h>

/*!
    abstract class DiskDriver
*/
class DiskDriver {

  public:
    virtual bool read(int lba, byte* buf) = 0;
    virtual bool write(int lba, byte* buf) = 0;
};

#endif
