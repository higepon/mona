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
    MFDCDriver();
    ~MFDCDriver();

  public:
    void printStatus() const;
    void interrupt();
  private:
    void initilize();
    void setFDCVersion();
    bool waitInterrupt();

  private:
    byte version_;
    static bool interrupt_;
};

extern MFDCDriver* gMFDCDriver;

#endif
