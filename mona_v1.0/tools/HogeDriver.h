/*!
    \file HogeDriver.h
    \brief HogeDriver for testing filesystem.

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author HigePon
    \version $Revision$
    \date create:2003/04/03 update:$Date$
*/

#ifndef _MONA_HOGE_DRIVER_
#define _MONA_HOGE_DRIVER_

#include <fstream>
#include <iostream>
#include "DiskDriver.h"

/*!
    Hoge Driver class
*/
class HogeDriver : public DiskDriver {
  public:
    HogeDriver(const char* file, long size);
    ~HogeDriver();

  public:
    bool read(int lba, byte* buf);
    bool write(int lba, byte* buf);

  private:

    std::fstream file_;
};

#endif
