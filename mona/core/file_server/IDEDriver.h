/*!
    \file  IDEDriver.h
    \brief IDEDriver

    Copyright (c) 2004 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2004/07/04 update:$Date$
*/
#ifndef _MONA_IDEDRVIER_
#define _MONA_IDEDRIVER_

#include <fat_write/IStorageDevice.h>
#include <sys/types.h>
#include <monapi/CString.h>

/*----------------------------------------------------------------------
    IDEDriver
----------------------------------------------------------------------*/
class IDEDriver : public IStorageDevice
{
public:
    IDEDriver(int irq_primary, int irq_secondary);
    virtual ~IDEDriver();

public:
    int open();
    int close();
    int read(uint32_t lba, void* buf, int size);
    int write(uint32_t lba, void* buf, int size);
    int ioctl(void* p);
    int getLogicalBlockSize();

public:
    void protocolInterrupt();
    bool selectDevice(int controller, int deviceNo);
    bool findDevice(int type, int detail, int* controller, int* deviceNo);
    int getLastError() const;
    void getLastErrorDetail(uint8_t* buffer);

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

    enum
    {
        SELECTION_ERROR = 1,
        DATA_READY_CHECK_ERROR,
        BUSY_TIMEOUT_ERROR,
        STATUS_ERROR
    };


private:
    typedef struct IDEDevice
    {
        int type;
        int typeDetail;
        int deviceNo;
        uint16_t sectorSize;
        MonAPI::CString name;
    };

    typedef struct IDEController
    {
        IDEController() : selectedDevice(NULL) {}
        uint8_t irq;
        int registers[10];
        IDEDevice devices[2];
        IDEDevice* selectedDevice;
    };

    typedef struct
    {
        uint8_t feature;
        uint8_t sectorCount;
        uint8_t sectorNumber;
        uint8_t cylinderLow;
        uint8_t cylinderHigh;
        uint8_t deviceNo;
        uint8_t command;
        bool drdyCheck;
    } ATACommand;

    typedef struct ATAPICommand
    {
        uint8_t feature;
        uint8_t deviceNo;
        uint8_t packet[12];
        uint16_t limit;
        void* buffer;
    };

private:

    /* I/O */
    void outp8(IDEController* controller, int reg, uint8_t value);
    uint8_t inp8(IDEController* controller, int reg);
    void inp16(IDEController* controller, uint16_t* data, int length);
    uint16_t inp16(IDEController* controller, int reg);
    void outp16(IDEController* controller, int reg, uint16_t value);
    void outp16(IDEController* controller, uint16_t* data, int length);

    /* flag utilities */
    bool waitBusyClear(IDEController* controller);
    bool waitBusyAndDataRequestBothClear(IDEController* controller);
    bool waitDrdySet(IDEController* controller);

    /* protocol: param IDEController. this layer returns error code */
    bool protocolAtaNoneData(IDEController* controller, ATACommand* command);
    bool protocolPacket(IDEController* controller, ATAPICommand* command);
    bool protocolPioDataIn(IDEController* controller, ATACommand* command, uint16_t count, void* buf);

    /* command : execute command using protocol function */
    bool commandIdleImmediate(IDEController* controller, int deviceNo);
    bool commandRequestSense(IDEController* controller);
    bool commandRead10(IDEController* controller, uint32_t lba, void* buffer, int size);
    bool commandIdentify(IDEController* controller, int deviceNo, uint16_t* buffer);

    /* private : functions */
    void initialize(IDEController* controller);
    void setDeviceTypeFirst(IDEController* controller, int deviceNo);
    void setDeviceTypeSecond(IDEController* controller, int deviceNo);
    bool selectDevice(IDEController* controller, int deviceNo);
    uint8_t deviceValue(int deviceNo) const;

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
        ATA_TIMEOUT    = 100000, // for Virtual PC changed from 100000 -> 10000
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
        REQUEST_SENSE_BUFFER_SIZE = 18,

        ATAPI_SECTOR_SIZE = 2048,
        ATA_SECTOR_SIZE   = 512
    };


private:
    IDEController controllers[2];
    IDEController* whichController;
    volatile void* atapiBuffer;
    volatile int atapiReadDone;
    volatile uint32_t atapiTransferSize;
    volatile uint32_t atapiTotalReadSize;
    uint8_t requestSenseBuffer[REQUEST_SENSE_BUFFER_SIZE];
    int lastError;

};



#endif
