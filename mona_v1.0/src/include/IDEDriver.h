/* IDE Driver test */
#include<types.h>
#include<DiskDriver.h>

#ifndef _MONA_MIDEDRIVER_MJT
#define _MONA_MIDEDRIVER_MJT

#define IDE_DEVICETYPE_HDD 1
#define IDE_DEVICETYPE_OTHER 99


class IDEDevice;

class IDEDriver {
  public:
    IDEDriver(unsigned int port);
    ~IDEDriver();

  public:
    static IDEDevice* Master;
    static IDEDevice* Slave;
    static bool HasMaster;
    static bool HasSlave;
  public:
    bool sendcmd(int cmd,byte *bfr,int bfrsize /* must be 2n */);
    bool cmdRead(byte *bfr,unsigned int count);
    bool senddevice(int drive);
    bool waithdc(unsigned long timeout);
    bool waitdata(unsigned long timeout);
    bool waitready(unsigned long timeout);
    bool initilize();
    bool setLBA(dword lba,unsigned int device);
    bool setCHS(dword lba,unsigned int device);
    void setCount(byte count);
  private:
    byte version_;
    static unsigned int control_;
    static unsigned int data_;
    static unsigned int error_;
    static unsigned int status_;
    static unsigned int sector_;
    static unsigned int count_;
    static unsigned int cylinderL_;
    static unsigned int cylinderH_;
    static unsigned int head_;
    static unsigned int status2_;
    static unsigned int driveaddr_;
    
    static byte results_[10];
    static int resultsLength_;
    static bool interrupt_;
    static byte* dmabuff_;
};

class IDEDevice : public DiskDriver {
  public:
    bool read(dword lba, byte* buf);
    bool write(dword lba, byte* buf);
    IDEDevice(IDEDriver *bus,unsigned int device);
    //~IDEDevice(){};
    static class IDEDriver* Bus;
    static unsigned int Heads;
    static unsigned int Tracks;
    static unsigned int SectorsPerTrack;
    static unsigned int BytesPerSector;
    static unsigned int TotalSize;
    static unsigned int DeviceType;
    static bool IsSurpportLBA;
  private:
    static unsigned int device_;
};

#endif
