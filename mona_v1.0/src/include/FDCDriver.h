/*!
    \file FDCDriver.h
    \brief Floppy Disk Controller driver for Multi Task

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author HigePon
    \version $Revision$
    \date create:2003/02/07 update:$Date$
*/

#include<types.h>
#include<VirtualConsole.h>
#include<DiskDriver.h>

#ifndef _MONA_MFDCDRIVER_
#define _MONA_MFDCDRIVER_

/*!
    Floppy Disk Controller class
*/
class FDCDriver : public DiskDriver {
  public:
    FDCDriver(VirtualConsole* console);
    ~FDCDriver();

  public:
    void printStatus(const char*) const;
    void interrupt();
    bool read(dword lba, byte* buf);
    bool write(dword lba, byte* buf);
    void motor(const bool on);
    void test();
    bool recalibrate();
  private:
    void initilize();
    void setFDCVersion();
    bool sendCommand(const byte command[], const byte length);
    bool waitInterrupt();
    bool checkMSR(byte expectedCondition, byte mask);
    bool checkMSR(byte expectedCondition);
    bool seek(byte track);
    bool senseInterrupt();
    bool readResults();
    void startDMA();
    void stopDMA();
    void setupDMARead(dword size);
    void setupDMAWrite(dword size);
    void printStatus(const byte msr, const char*) const;
    void printDMACStatus(const byte status, const char*) const;
    void lbaToTHS(int lba, byte& track, byte& head, byte& sector);
    void waitPrint(const char* msg);
    bool read(byte track, byte head, byte sector);
    bool write(byte track, byte head, byte sector);
    bool writeID(byte track, byte head, byte data);
  private:
    byte version_;
    static byte results_[10];
    static int resultsLength_;
    static bool interrupt_;
    static VirtualConsole* console_;
    static byte* dmabuff_;
};

extern FDCDriver* gFDCDriver;

#endif
