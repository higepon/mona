/*!
    \file MFDCDriver.h
    \brief Floppy Disk Controller driver for Multi Task

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author HigePon
    \version $Revision$
    \date create:2003/02/07 update:$Date$
*/

#include<monaTypes.h>

#ifndef _MONA_MFDCDRIVER_
#define _MONA_MFDCDRIVER_

/*!
    Floppy Disk Controller class
*/
class MFDCDriver {
  public:
    MFDCDriver(const byte drive);
    ~MFDCDriver();

  public:

  private:
    void initilize(const byte drive);

};

#endif
