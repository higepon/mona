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

#include <fstream>
#include <iostream>
/*!
    Disk Driver class
*/
class DiskDriver {
  public:
    DiskDriver(const char* file, long size);
    ~DiskDriver();

  public:
    bool read(int sector, char* buf);
    bool write(int sector, char* buf);

  private:

    std::fstream file_;
};

#endif
