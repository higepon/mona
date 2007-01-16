/*
    \file FDCDriver.h
    \brief Floppy Disk Controller driver for Multi Task

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author HigePon
    \version $Revision$
    \date create:2003/02/07 update:$Date$
*/

#include <sys/types.h>
#include <monapi/io.h>
#include <monapi.h>
#include <monapi/messages.h>
#include <fat_write/IStorageDevice.h>

#ifndef _FDCDRIVER_
#define _FDCDRIVER_

/*!
    Floppy Disk Controller class
*/
class FDCDriver : public IStorageDevice
{
public:
    FDCDriver();
    virtual ~FDCDriver();

public:
    int open();
    int close();
    int read(uint32_t lba, void* buf, int size);
    int write(uint32_t lba, void* buf, int size);
    int ioctl(void* p);
    bool checkDiskChange();
    bool read(uint32_t lba, uint8_t* buf);
    bool write(uint32_t lba, uint8_t* buf);
    bool recalibrate();
    void motor(const bool on);
    void motorAutoOff();
    bool isInserted(int drive);

private:
    void initilize();
    bool seek(uint8_t track);
    bool waitInterrupt();
    void waitStatus(uint8_t expected);
    void waitStatus(uint8_t mask, uint8_t expected);
    bool sendCommand(const uint8_t command[], const uint8_t length);
    uint8_t getResult();
    bool senseInterrupt();
    bool readResults();
    void startDMA();
    void stopDMA();
    void setupDMARead(uint32_t size);
    void setupDMAWrite(uint32_t size);
    void lbaToTHS(int lba, uint8_t& track, uint8_t& head, uint8_t& sector);
    bool read(uint8_t track, uint8_t head, uint8_t sector);
    bool write(uint8_t track, uint8_t head, uint8_t sector);

    inline void delay(int n)
    {
        for (int i = 0; i < n; i++)
        {
            inp8(0x80);
        }
    }
private:
    uint8_t results[10];
    int motorCount;
    int resultsLength;
    int currentTrack;
    uint8_t* dmabuff;

private:
    enum
    {
        /* definition DOR */
        FDC_MOTA_START   = 0x10,
        FDC_DMA_ENABLE   = 0x08,
        FDC_REST_RESET   = 0x00,
        FDC_REST_ENABLE  = 0x04,
        FDC_DR_SELECT_A  = 0x00,

        /* definition MSR */
        FDC_MRQ_READY     = 0x80,
        FDC_DIO_TO_CPU    = 0x40,
        FDC_NDMA_NOT_DMA  = 0x20,
        FDC_BUSY_ACTIVE   = 0x10,
        FDC_ACTD_ACTIVE   = 0x08,
        FDC_ACTC_ACTIVE   = 0x04,
        FDC_ACTB_ACTIVE   = 0x02,
        FDC_ACTA_ACTIVE   = 0x01,

        /* port address */
        FDC_DOR_PRIMARY    = 0x3f2,
        FDC_DOR_SECONDARY  = 0x372,
        FDC_MSR_PRIMARY    = 0x3f4,
        FDC_MSR_SECONDARY  = 0x374,
        FDC_DR_PRIMARY     = 0x3f5,
        FDC_DR_SECONDARY   = 0x375,
        FDC_CCR_PRIMARY    = 0x3f7,
        FDC_CCR_SECONDARY  = 0x377,

        FDC_DMA_S_SMR      = 0x0a,
        FDC_DMA_S_MR       = 0x0b,
        FDC_DMA_S_CBP      = 0x0c,
        FDC_DMA_S_BASE     = 0x04,
        FDC_DMA_S_COUNT    = 0x05,
        FDC_DMA_PAGE2      = 0x81,

        /* summary */
        FDC_DOR_RESET   = 0,
        FDC_START_MOTOR = (FDC_DMA_ENABLE | FDC_MOTA_START | FDC_REST_ENABLE | FDC_DR_SELECT_A),
        FDC_STOP_MOTOR  = (FDC_DMA_ENABLE | FDC_REST_ENABLE | FDC_DR_SELECT_A),

        /* FDC Commands */
        FDC_COMMAND_SEEK            = 0x0f,
        FDC_COMMAND_SENSE_INTERRUPT = 0x08,
        FDC_COMMAND_SPECIFY         = 0x03,
        FDC_COMMAND_READ            = 0xe6, // bochs & VPC
        //FDC_COMMAND_READ          =  0x46,

        /* time out */
        FDC_RETRY_MAX     = 600000,
        FDC_DMA_BUFF_SIZE = 512,
        IRQ_NUMBER        = 6,
    };
};

/* definition on/off */
#define ON  true
#define OFF false

#endif
