/*!
    \file  IDEDriver.h
    \brief IDEDriver

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2004/07/04 update:$Date$
*/
#ifndef _MONA_IDEDRVIER_
#define _MONA_IDEDRIVER_

#include <fat_write/IStorageDevice.h>
/*----------------------------------------------------------------------
    IDEDriver
----------------------------------------------------------------------*/
class IDEController
{
public:
    int selectedDevice;
    int registers[10];
};

typedef struct
{
    byte feature;
    byte sectorCount;
    byte sectorNumber;
    byte cylinderLow;
    byte cylinderHigh;
    byte device;
    byte command;
    byte drdyCheck;
} ATACommand;

class IDEDriver : public IStorageDevice
{
public:
    IDEDriver();
    virtual ~IDEDriver();

public:
    int open();
    int close();
    int read(dword lba, void* buf, int size);
    int write(dword lba, void* buf, int size);
    int ioctl(void* p);
    int setDevice(int ide, int device);
    int getDeviceType(int ide, int device);

public:
    enum
    {
        PRIMARY   = 0,
        SECONDARY = 1,
        MASTER    = 0,
        SLAVE     = 1,
        DEVICE_UNKNOWN,
        DEVICE_NON,
        DEVICE_ATA,
        DEVICE_ATAPI
    };

private:
    void outp8(IDEController* ide, int reg, byte value);
    byte inp8(IDEController* ide, int reg);
    word inp16(IDEController* ide, int reg);
    bool waitBusyClear(IDEController* ide);
    bool waitDrdySet(IDEController* ide);
    bool selectDevice(IDEController* ide, int device);
    word getSignature(IDEController* ide, int device);
    void read(IDEController* ide, word length, void* buf);
    int sendPioDataInCommand(IDEController* ide, ATACommand* command, word count, void* buf);
    int identify(IDEController* ide, int device, int type, void* buf);
    int judgeDevice(IDEController* ide, int device);
    int read(IDEController* ide, int device, dword lba, byte blocknum, void* buff);

    enum
    {
        ATA_DTR      = 0,
        ATA_ERR      = 1,
        ATA_FTR      = 1,
        ATA_SCR      = 2,
        ATA_IRR      = 2,
        ATA_SNR      = 3,
        ATA_CLR      = 4,
        ATA_BLR      = 4,
        ATA_CHR      = 5,
        ATA_BHR      = 5,
        ATA_DHR      = 6,
        ATA_STR      = 7,
        ATA_CMR      = 7,
        ATA_ASR      = 8,
        ATA_DCR      = 8,
        ATA_TIMEOUT  = 100000,
        BIT_BSY      = 0x80,
        BIT_DRDY     = 0x40,
        BIT_DRQ      = 8,
        BIT_ABRT     = 4,
        BIT_CHK      = 1,
        BIT_ERR      = 1,
        BIT_IO       = 2,
        BIT_CD       = 1,
        RETRY_MAX    = 2,
        DEV_HEAD_OBS = 0xa0,
        LBA_FLG      = 0x40
    };

private:
    bool deviceOK;
    IDEController ide[2];
    int whichIde;
    int whichDevice;

};

#endif
