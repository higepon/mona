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
#include <sys/types.h>
#include <monapi/Cstring.h>

/*----------------------------------------------------------------------
    IDEDriver
----------------------------------------------------------------------*/
class IDEDriver
{
public:
    IDEDriver();
    virtual ~IDEDriver();

public:
    void printDebug();
    bool selectDevice(int controller, int deviceNo);
    int read(dword lba, void* buffer, int size);
    bool findDevice(int type, int detail, int* controller, int* deviceNo);

public:
    enum
    {
        PRIMARY   = 0,
        SECONDARY = 1,
        MASTER    = 0,
        SLAVE     = 1,
        DEVICE_UNKNOWN,
        DEVICE_NONE,
        DEVICE_ATA,
        DEVICE_ATAPI
    };

private:
    typedef struct IDEDevice
    {
        int type;
        int typeDetail;
        int deviceNo;
        dword dataTransferSize;
        MonAPI::CString name;
    };

    typedef struct IDEController
    {
        IDEController() : selectedDevice(NULL) {}
        int registers[10];
        IDEDevice devices[2];
        IDEDevice* selectedDevice;
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

    typedef struct ATAPICommand
    {
        byte feature;
        byte device;
        byte packet[12];
    };

private:
    void resetAndIdentify(IDEController* controller);
    void identify(IDEController* controller, int deviceNo);
    void identifyDetail(IDEController* controller, int deviceNo);
    bool waitBusyClear(IDEController* controller);
    int judgeDeviceType(byte high, byte low);
    void printDebug(IDEController* controller, int deviceNo);
    bool waitBusyAndDataRequestBothClear(IDEController* controller);
    bool waitDrdySet(IDEController* controller);
    int sendPioDataInCommand(IDEController* controller, ATACommand* command, word count, void* buf);
    int sendPacketCommand(IDEController* controller, ATAPICommand* command, word limit, void* buffer);
    bool selectDevice(IDEController* controller, int deviceNo);
    int readATAPI(IDEController* controller, dword lba, void* buffer, int size);
    dword requestSense(IDEController* controller);

private:
    void outp8(IDEController* controller, int reg, byte value);
    byte inp8(IDEController* controller, int reg);
    void inp16(IDEController* controller, word* data, int length);
    word inp16(IDEController* controller, int reg);
    void outp16(IDEController* controller, int reg, word value);
    void outp16(IDEController* controller, word* data, int length);


private:
    IDEController controllers[2];
    IDEController* whichController;

private:
    enum
    {
        ATA_DTR        = 0,
        ATA_ERR        = 1,
        ATA_FTR        = 1,
        ATA_SCR        = 2,
        ATA_IRR        = 2,
        ATA_SNR        = 3,
        ATA_CLR        = 4,
        ATA_BLR        = 4,
        ATA_CHR        = 5,
        ATA_BHR        = 5,
        ATA_DHR        = 6,
        ATA_STR        = 7,
        ATA_CMR        = 7,
        ATA_ASR        = 8,
        ATA_DCR        = 8,
        ATA_TIMEOUT    = 1000000,
        BIT_BSY        = 0x80,
        BIT_DRDY       = 0x40,
        BIT_DRQ        = 8,
        BIT_ABRT       = 4,
        BIT_CHK        = 1,
        BIT_ERR        = 1,
        BIT_IO         = 2,
        BIT_CD         = 1,
        RETRY_MAX      = 2,
        DEV_HEAD_OBS   = 0xa0,
        LBA_FLG        = 0x40,
    };

};



#endif
