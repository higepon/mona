/*!
    \file DiskDriver.h
    \brief DiskDriver for testing filesystem.

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author HigePon
    \version $Revision$
    \date create:2003/04/03 update:$Date$
*/

#ifndef _MONA_DISK_DRIVER_
#define _MONA_DISK_DRIVER_

/*!
    Disk Driver class
*/
class DiskDriver {
  public:
    DiskDriver(const char* file, long size);
    ~DiskDriver();

  public:
    bool read(char* buf, int sector);
    bool write(char* buf, int sector);
};

#endif
