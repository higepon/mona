/*!
    \file  IDEDriver.cpp
    \brief IDEDriver

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2004/11/14 update:$Date$
*/

#include "IDEDriver.h"
#include <monapi.h>
#include <Assert.h>
#include <monapi/io.h>

using namespace MonAPI;

//#define DEBUG_READ_TRACE

/*----------------------------------------------------------------------
    IDEDRIVER
----------------------------------------------------------------------*/
IDEDriver::IDEDriver()
{
    this->controllers[PRIMARY].registers[ATA_DTR] = 0x1f0;
    this->controllers[PRIMARY].registers[ATA_ERR] = 0x1f1;
    this->controllers[PRIMARY].registers[ATA_SCR] = 0x1f2;
    this->controllers[PRIMARY].registers[ATA_SNR] = 0x1f3;
    this->controllers[PRIMARY].registers[ATA_CLR] = 0x1f4;
    this->controllers[PRIMARY].registers[ATA_CHR] = 0x1f5;
    this->controllers[PRIMARY].registers[ATA_DHR] = 0x1f6;
    this->controllers[PRIMARY].registers[ATA_STR] = 0x1f7;
    this->controllers[PRIMARY].registers[ATA_ASR] = 0x3f6;

    this->controllers[SECONDARY].registers[ATA_DTR] = 0x170;
    this->controllers[SECONDARY].registers[ATA_ERR] = 0x171;
    this->controllers[SECONDARY].registers[ATA_SCR] = 0x172;
    this->controllers[SECONDARY].registers[ATA_SNR] = 0x173;
    this->controllers[SECONDARY].registers[ATA_CLR] = 0x174;
    this->controllers[SECONDARY].registers[ATA_CHR] = 0x175;
    this->controllers[SECONDARY].registers[ATA_DHR] = 0x176;
    this->controllers[SECONDARY].registers[ATA_STR] = 0x177;
    this->controllers[SECONDARY].registers[ATA_ASR] = 0x376;

    /* initialize controllers */
    initialize(&controllers[PRIMARY]);
    initialize(&controllers[SECONDARY]);

    this->whichController = NULL;
    this->atapiBuffer     = NULL;
    this->atapiReadDone   = true;
}

IDEDriver::~IDEDriver()
{
}

/*----------------------------------------------------------------------
    IDEDRIVER : for user operation
----------------------------------------------------------------------*/
bool IDEDriver::selectDevice(int controller, int deviceNo)
{
    if (controller != PRIMARY && controller != SECONDARY) return false;
    if (deviceNo != MASTER && deviceNo != SLAVE) return false;

    IDEDevice* device = &controllers[controller].devices[deviceNo];

    if (device->type == DEVICE_NONE || device->type == DEVICE_UNKNOWN) return false;

    controllers[controller].selectedDevice = device;
    whichController = &controllers[controller];
    return true;
}

bool IDEDriver::findDevice(int type, int detail, int* controller, int* deviceNo)
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            IDEDevice* device = &controllers[i].devices[j];

            if (type != device->type || detail != device->typeDetail) continue;

            *controller = i;
            *deviceNo = j;
            return true;
        }
    }

    return false;
}

int IDEDriver::read(dword lba, void* buffer, int size)
{
#ifdef DEBUG_READ_TRACE
    void* buffer2 = buffer;
    logprintf("read lba=%d size=%x start\n", lba, size);
#endif

    if (this->whichController == NULL) return 1;

    if (this->whichController->selectedDevice->type == DEVICE_ATAPI)
    {
        int count = (size + 0xf800 - 1) / 0xf800;

        for (int i = 0; i < count; i++)
        {
            int readSize;
            bool readResult;
            if (i == count - 1)
            {
                readSize = size - 0xf800 * i;
            }
            else
            {
                readSize = 0xf800;
            }

            for (int j = 0; j < 20; j++)
            {
                readResult = commandRead10(this->whichController, lba, buffer, readSize);
                if (readResult) break;
            }

            buffer = (void*)((byte*)buffer + readSize);

            if (!readResult) return getLastError();
        }

#ifdef DEBUG_READ_TRACE
        byte* p = (byte*)buffer2;
        for (int k= 0; k < size; k++)
        {
            logprintf("[%d:%x]", k, p[k]);
        }
        logprintf("\n\n");
#endif

        return 0;
    }
    else if (this->whichController->selectedDevice->type == DEVICE_ATA)
    {
        return 4;
    }
    else
    {
        return 5;
    }
}

int IDEDriver::open()
{
    return 0;
}

int IDEDriver::close()
{
    return 0;
}

int IDEDriver::write(dword lba, void* buf, int size)
{
    return 0;
}

int IDEDriver::ioctl(void* p)
{
    return 0;
}

int IDEDriver::getLogicalBlockSize()
{
    return whichController->selectedDevice->sectorSize;
}
/*----------------------------------------------------------------------
    IDEDRIVER : error information
----------------------------------------------------------------------*/
int IDEDriver::getLastError() const
{
    return this->lastError;
}

void IDEDriver::getLastErrorDetail(byte* buffer)
{
    commandRequestSense(whichController);
    memcpy(buffer, this->requestSenseBuffer, REQUEST_SENSE_BUFFER_SIZE);
}

/*----------------------------------------------------------------------
    IDEDRIVER : I/O
----------------------------------------------------------------------*/
void IDEDriver::outp8(IDEController* controller, int reg, byte value)
{
    ::outp8(controller->registers[reg], value);
}

byte IDEDriver::inp8(IDEController* controller, int reg)
{
    return ::inp8(controller->registers[reg]);
}


word IDEDriver::inp16(IDEController* controller, int reg)
{
    return ::inp16(controller->registers[reg]);
}

void IDEDriver::outp16(IDEController* controller, int reg, word value)
{
    ::outp16(controller->registers[reg], value);
}

void IDEDriver::outp16(IDEController* controller, word* data, int length)
{
    for(int i=0; i < length; i++)
    {
        outp16(controller, ATA_DTR, *data);
        data++;
    }
}

void IDEDriver::inp16(IDEController* controller, word* data, int size)
{
    int length = size / 2;

    if (data == NULL)
    {
        for (int i = 0; i < length; i++)
        {
            inp16(controller, ATA_DTR);
        }
    }
    else
    {
        for (int i = 0; i < length; i++)
        {
            *data = inp16(controller, ATA_DTR);
            data++;
        }
    }
}

/*----------------------------------------------------------------------
    IDEDRIVER : flag utilities
----------------------------------------------------------------------*/
bool IDEDriver::waitBusyAndDataRequestBothClear(IDEController* controller)
{
    dword i;
    for (i = 0; i < ATA_TIMEOUT; i++)
    {
        byte status = inp8(controller, ATA_ASR);
        if ((status & BIT_BSY) == 0 && (status & BIT_DRQ) == 0) break;
    }
    return (i != ATA_TIMEOUT);
}

bool IDEDriver::waitBusyClear(IDEController* controller)
{
    dword i;
    for (i = 0; i < ATA_TIMEOUT; i++)
    {
        byte status = inp8(controller, ATA_ASR);
        if ((status & BIT_BSY) == 0) break;
    }
    return (i != ATA_TIMEOUT);
}

bool IDEDriver::waitDrdySet(IDEController* controller)
{
    dword i;

    for (i = 0; i < ATA_TIMEOUT; i++)
    {
        byte status = inp8(controller, ATA_ASR);
        if (status & BIT_DRDY) break;
    }

    return (i != ATA_TIMEOUT);
}

/*----------------------------------------------------------------------
    IDEDRIVER : protocol
----------------------------------------------------------------------*/
bool IDEDriver::protocolPacket(IDEController* controller, ATAPICommand* command)
{
    outp8(controller, ATA_DCR, 0x8);  /* use interrupt */

    atapiBuffer        = command->buffer;
    atapiReadDone      = false;
    atapiTotalReadSize = command->limit;

    if (!selectDevice(controller, command->deviceNo))
    {
        this->lastError = SELECTION_ERROR;
        return false;
    }

    /* packet command */
    outp8(controller, ATA_FTR, command->feature);
    outp8(controller, ATA_SCR, 0);
    outp8(controller, ATA_BLR, (byte)(command->limit & 0xff));
    outp8(controller, ATA_BHR, (byte)(command->limit >> 8));
    outp8(controller, ATA_CMR, 0xa0);
    sleep(1);

    dword i;
    for (i = 0; i < ATA_TIMEOUT; i++)
    {
        byte status = inp8(controller, ATA_ASR);

        if ((status & BIT_BSY) != 0) continue;
        if ((status & BIT_CHK) != 0)
        {
            atapiBuffer = NULL;
            inp8(controller, ATA_ERR); /* must? */
            this->lastError = STATUS_ERROR;
            return false;
        }

        byte reason = inp8(controller, ATA_IRR);
        if (((status & BIT_DRQ) != 0) && ((reason & BIT_IO) == 0) && ((reason & BIT_CD) != 0)) break;
    }

    if (i == ATA_TIMEOUT)
    {
        atapiBuffer = NULL;
        this->lastError = BUSY_TIMEOUT_ERROR;
        return false;
    }

    outp16(controller, (word*)command->packet, 6);
    for (i = 0; i < ATA_TIMEOUT; i++)
    {
        byte status = inp8(controller, ATA_ASR);

        if ((status & BIT_BSY) != 0) continue;

        if ((status & BIT_CHK) != 0)
        {
            atapiBuffer = NULL;
            this->lastError = STATUS_ERROR;
            return false;
        }
        if (atapiReadDone) break;
    }

    inp8(controller, ATA_STR);

    if (i == ATA_TIMEOUT)
    {
        atapiBuffer = NULL;
        this->lastError = BUSY_TIMEOUT_ERROR;
        return false;
    }

    return true;
}

bool IDEDriver::protocolAtaNoneData(IDEController* controller, ATACommand* command)
{
    /* select device */
    if (!selectDevice(controller, command->deviceNo))
    {
        this->lastError = SELECTION_ERROR;
        return false;
    }

    outp8(controllers, ATA_DCR, 0x2); /* no interrupt */
    outp8(controllers, ATA_FTR, command->feature);
    outp8(controllers, ATA_SCR, command->sectorCount);
    outp8(controllers, ATA_SNR, command->sectorNumber);
    outp8(controllers, ATA_CLR, command->cylinderLow);
    outp8(controllers, ATA_CHR, command->cylinderHigh);

    /* data ready check */
    if (!waitDrdySet(controller))
    {
        this->lastError = DATA_READY_CHECK_ERROR;
        return false;
    }

    outp8(controllers, ATA_CMR, command->command);
    sleep(1);

    /* wait busy clear */
    if (!waitBusyClear(controller))
    {
        this->lastError = BUSY_TIMEOUT_ERROR;
        return false;
    }

    inp8(controllers, ATA_ASR); /* read once */

    /* check error */
    byte status = inp8(controllers, ATA_STR);
    if (status & BIT_ERR)
    {
        inp8(controllers, ATA_ERR); /* must read ? */
        this->lastError = STATUS_ERROR;
        return false;
    }

    return true;
}

bool IDEDriver::protocolPioDataIn(IDEController* controller, ATACommand* command, word count, void* buf)
{
    word* p = (word*)buf;

    if (!selectDevice(controller, command->deviceNo))
    {

        this->lastError = SELECTION_ERROR;
        return false;
    }

    outp8(controller, ATA_DCR, 0x02);                  /* not use interrupt */
    outp8(controller, ATA_FTR, command->feature);      /* feature           */
    outp8(controller, ATA_SCR, command->sectorCount);  /* sector count      */
    outp8(controller, ATA_SNR, command->sectorNumber); /* sector number     */
    outp8(controller, ATA_CLR, command->cylinderLow);  /* cylinderLow       */
    outp8(controller, ATA_CHR, command->cylinderHigh); /* cylinderHigh      */

    /* drdy check */
    if (command->drdyCheck && !waitDrdySet(controller))
    {
        this->lastError = DATA_READY_CHECK_ERROR;
        return false;
    }

    outp8(controller, ATA_CMR, command->command);
    sleep(1);

    /* read atlternate status once */
    inp8(controller, ATA_ASR);

    /* read */
    for (int i = 0; i < count; i++, p+=256)
    {
        if (!waitBusyClear(controller))
        {
            this->lastError = BUSY_TIMEOUT_ERROR;
            return false;
        }

        byte status = inp8(controller, ATA_STR);

        /* command error */
        if ((status & BIT_ERR) != 0)
        {
            this->lastError = STATUS_ERROR;
            return false;
        }

        /* data not ready */
        if ((status & BIT_DRQ) == 0)
        {
            this->lastError = DATA_READY_CHECK_ERROR;
            return false;
        }

        /* data read */
        inp16(controller, p, 256);
    }

    inp8(controller, ATA_ASR);
    byte status = inp8(controller, ATA_STR);

    if (status & BIT_ERR)
    {
        inp8(controller, ATA_ERR); /* must ? */
        this->lastError = STATUS_ERROR;
        return false;
    }
    return true;
}

void IDEDriver::protocolInterrupt()
{
    byte status = inp8(whichController, ATA_STR);
    byte reason = inp8(whichController, ATA_IRR);

    /* read */
    if (((reason & BIT_IO) != 0) && ((reason & BIT_CD) == 0) && ((status & BIT_DRQ) != 0))
    {
        word transferSize = (inp8(whichController, ATA_BHR) << 8) | inp8(whichController, ATA_BLR);
        atapiTransferSize += transferSize;

        if (atapiTransferSize > atapiTotalReadSize)
        {
            inp16(whichController, NULL, transferSize);
        }
        else
        {
            inp16(whichController, (word*)atapiBuffer, transferSize);
            atapiBuffer = (void*)((byte*)atapiBuffer + transferSize);
        }
    }

    /* read / write done */
    if (((reason & BIT_IO)!=0) && ((reason & BIT_CD) != 0) && ((status & BIT_DRQ) == 0))
    {
        atapiReadDone = true;
    }
}

/*----------------------------------------------------------------------
    IDEDRIVER : execute command using protocol function
----------------------------------------------------------------------*/
bool IDEDriver::commandIdleImmediate(IDEController* controller, int deviceNo)
{
    ATACommand command;
    memset(&command, 0, sizeof(command));

    command.deviceNo  = deviceNo;
    command.command   = controller->selectedDevice->type == DEVICE_ATA ? 0xe3 : 0xe1;
    command.drdyCheck = true;

    return protocolAtaNoneData(controller, &command);
}

bool IDEDriver::commandRequestSense(IDEController* controller)
{
    ATAPICommand command;
    memset(&command, 0, sizeof(command));

    command.feature   = 0;
    command.deviceNo  = controller->selectedDevice->deviceNo;
    command.packet[0] = 0x03;
    command.packet[4] = REQUEST_SENSE_BUFFER_SIZE;
    command.limit     = REQUEST_SENSE_BUFFER_SIZE;
    command.buffer    = this->requestSenseBuffer;
    atapiTransferSize = 0;

    memset(requestSenseBuffer, 0, REQUEST_SENSE_BUFFER_SIZE);
    return protocolPacket(controller, &command);
}

bool IDEDriver::commandRead10(IDEController* controller, dword lba, void* buffer, int size)
{
    ATAPICommand command;
    memset(&command, 0, sizeof(command));

    /* sector count */
    int count = (size + ATAPI_SECTOR_SIZE - 1) / ATAPI_SECTOR_SIZE;

    command.feature   = 0;
    command.deviceNo  = controller->selectedDevice->deviceNo;
    command.packet[0] = 0x28;
    command.packet[2] = (lba >>  24) & 0xff;
    command.packet[3] = (lba >>  16) & 0xff;
    command.packet[4] = (lba >>   8) & 0xff;
    command.packet[5] = (lba       ) & 0xff;
    command.packet[7] = (count >> 8) & 0xff;
    command.packet[8] = (count     ) & 0xff;
    command.limit     = ATAPI_SECTOR_SIZE * count;
    command.buffer    = buffer;
    atapiTransferSize = 0;

    return protocolPacket(controller, &command);
}

bool IDEDriver::commandIdentify(IDEController* controller, int deviceNo, word* buffer)
{
    ATACommand command;
    memset(&command, 0, sizeof(command));

    IDEDevice* device = &controller->devices[deviceNo];

    command.deviceNo = deviceNo;
    if (device->type == DEVICE_ATA)
    {
        command.drdyCheck = true;
        command.command   = 0xec;
    }
    else
    {
        command.drdyCheck = false;
        command.command   = 0xa1;
    }

    bool commandOK = protocolPioDataIn(controller, &command, 1, buffer);

    if (!commandOK) return false;

    word* p = buffer;
    for(int i = 0; i < 256; i++)
    {
        word value = *p;
        *p = ((value >> 8) & 0xff) | ((value << 8) & 0xff00);
        p++;
    }

    return true;
}

/*----------------------------------------------------------------------
    IDEDRIVER :initialize functions
----------------------------------------------------------------------*/
void IDEDriver::initialize(IDEController* controller)
{
    /* software reset */
    outp8(controller, ATA_DCR, 0x06);
    sleep(5);

    /* no interrupt */
    outp8(controller, ATA_DCR, 0x02);
    sleep(5);

    setDeviceTypeFirst(controller, MASTER);
    setDeviceTypeFirst(controller, SLAVE);
    setDeviceTypeSecond(controller, MASTER);
    setDeviceTypeSecond(controller, SLAVE);
}

/*
    call only after software reset
*/
void IDEDriver::setDeviceTypeFirst(IDEController* controller, int deviceNo)
{
    dword l;
    byte c;
    byte c1 = 0xff; /* unknown signature */
    byte c2 = 0xff; /* unknown signature */
   bool timeout;

    IDEDevice* device = &controller->devices[deviceNo];
    device->deviceNo = deviceNo;

    for (l = 0; l < RETRY_MAX; l++)
    {
        /* select device */
        outp8(controller, ATA_DHR, deviceValue(deviceNo));
        sleep(10);

        c = inp8(controller, ATA_STR);
        if (c == 0xff) break;

        timeout = !waitBusyClear(controller);
        if (timeout) break;

        /* bad device */
        byte error = inp8(controller, ATA_ERR);
        if (deviceNo == MASTER && (error & 0x7f) != 1)
        {
            device->type = DEVICE_UNKNOWN;
            return;
        }
        else if (deviceNo == SLAVE && error != 1)
        {
            device->type = DEVICE_UNKNOWN;
            return;
        }

        c = inp8(controller, ATA_DHR);
        if ((c & (deviceNo << 4)) == (deviceNo << 4))
        {
            c1 = inp8(controller, ATA_CLR);
            c2 = inp8(controller, ATA_CHR);
            break;
        }
    }

    switch(c1 | (c2 << 8))
    {
    case 0xEB14:
        device->type = DEVICE_ATAPI;
        break;
    case 0:
        device->type = DEVICE_ATA;
        break;
    default:
        device->type = DEVICE_NONE;
        break;
    }
}

void IDEDriver::setDeviceTypeSecond(IDEController* controller, int deviceNo)
{
    word buffer[256]; /* identify buffer */
    IDEDevice* device = &controller->devices[deviceNo];

    if (!waitBusyClear(controller))
    {
        device->type = DEVICE_NONE;
        return;
    }

    byte l;
    for (l = 0; l < RETRY_MAX; l++)
    {
        bool firstResult = commandIdentify(controller, deviceNo, buffer);
        int firstError   = getLastError();
        sleep(5);
        bool secondResult = commandIdentify(controller, deviceNo, buffer);
        int secondError   = getLastError();

        if (firstResult && secondResult)
        {
            break;
        }
        else if (!firstResult && !secondResult)
        {
            if (firstError != secondError) continue;
            if (firstError == SELECTION_ERROR || firstError == BUSY_TIMEOUT_ERROR || firstError == DATA_READY_CHECK_ERROR)
            {
                device->type = DEVICE_NONE;
                break;
            }
        }
    }

    if (l == RETRY_MAX)
    {
        device->type = DEVICE_UNKNOWN;
    }

    /* information */
    switch(device->type)
    {
    case DEVICE_ATA:
        device->name = CString((const char*)((byte*)buffer + 54), 40);
        device->typeDetail = -1;
        device->sectorSize = ATA_SECTOR_SIZE;
        break;

    case DEVICE_ATAPI:
        device->name       = CString((const char*)((byte*)buffer + 54), 40);
        device->typeDetail = buffer[0] & 0x1f;
        device->sectorSize = ATAPI_SECTOR_SIZE;
        break;

    case DEVICE_NONE:
        device->name       = "none";
        device->typeDetail = -1;
        break;

    case DEVICE_UNKNOWN:
        device->name       = "unknown";
        device->typeDetail = -1;
        break;
    }
}

bool IDEDriver::selectDevice(IDEController* controller, int deviceNo)
{
    if (whichController != NULL)
    {
        IDEDevice* device = whichController->selectedDevice;
        if (whichController == controller && device->deviceNo == deviceNo)
        {
            outp8(controller, ATA_DHR, deviceValue(deviceNo));
            sleep(10);
            return true;
        }
    }

    if (!waitBusyAndDataRequestBothClear(controller)) return false;

    /* select device */
    outp8(controller, ATA_DHR, deviceValue(deviceNo));
    sleep(10);

    if (!waitBusyAndDataRequestBothClear(controller)) return false;

    whichController = controller;
    whichController->selectedDevice = &controller->devices[deviceNo];

    return true;
}

byte IDEDriver::deviceValue(int deviceNo) const
{
    return (byte)(DEV_HEAD_OBS | (deviceNo << 4));
}
