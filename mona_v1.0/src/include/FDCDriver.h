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
#include<Process.h>
#include<VirtualConsole.h>
#include<IStorageDevice.h>

#ifndef _MONA_MFDCDRIVER_
#define _MONA_MFDCDRIVER_

/*!
    Floppy Disk Controller class
*/
class FDCDriver : public IStorageDevice {
  public:
    FDCDriver();
    virtual ~FDCDriver();

  public:
    int open();
    int close();
    int read(int lba, void* buf, int size);
    int write(int lba, void* buf, int size);
    int ioctl(void* p);
    bool checkDiskChange();
    void interrupt();
    bool read(dword lba, byte* buf);
    bool write(dword lba, byte* buf);
    bool recalibrate();
    bool seek(byte track);//private
    void motor(const bool on);
    void motorAutoOff();
    inline Thread* getWaitThread()
    {
        return (Thread*)waitThread;
    }

    inline void setWaitThread(Thread* thread)
    {
        this->waitThread = thread;
    }

    inline bool interrupted() const
    {
        return interrupt_;
    }

  private:
    void initilize();
    void waitInterrupt(bool yield);
    void waitStatus(byte expected);
    void waitStatus(byte mask, byte expected);
    bool sendCommand(const byte command[], const byte length);
    byte getResult();
    bool senseInterrupt();
    bool readResults();
    void startDMA();
    void stopDMA();
    void setupDMARead(dword size);
    void setupDMAWrite(dword size);
    void lbaToTHS(int lba, byte& track, byte& head, byte& sector);
    bool read(byte track, byte head, byte sector);
    bool write(byte track, byte head, byte sector);
    static Thread* waitThread;
  private:
    byte results_[10];
    int motorCount_;
    int resultsLength_;
    int currentTrack_;
    static volatile bool interrupt_;
    byte* dmabuff_;
};

extern FDCDriver* gFDCDriver;

/* delay */
#define delay(n) {for (int i = 0; i < n; i++) inportb(0x80);}

/* definition on/off */
#define ON  true
#define OFF false

#endif
