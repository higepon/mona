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

using namespace MonAPI;

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

    resetAndIdentify(&this->controllers[PRIMARY]);
    resetAndIdentify(&this->controllers[SECONDARY]);

    this->whichController = NULL;
}


IDEDriver::~IDEDriver()
{

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

int IDEDriver::sendPacketCommand(IDEController* controller, ATAPICommand* command, word limit, void* buffer)
{
    outp8(controller, ATA_DCR, 0xa);  /* no interrupt */

    printf("controller=%d\n", controller);
    printf("command->deviceNo=%d\n", command->device);
    if (!selectDevice(controller, command->device)) return 1;

    /* send packet command */
    outp8(controller, ATA_FTR, command->feature);
    outp8(controller, ATA_SCR, 0);
    outp8(controller, ATA_BLR, (byte)(limit & 0xff));
    outp8(controller, ATA_BHR, (byte)(limit >> 8));
    outp8(controller, ATA_CMR, 0xa0);
    sleep(1);

    dword i;
    for (i = 0; i < ATA_TIMEOUT; i++)
    {
        byte status = inp8(controller, ATA_ASR);

        if ((status & BIT_BSY) != 0) continue;
        if ((status & BIT_CHK) != 0)
        {
            printf("%s:%d error=%x\n", __FILE__, __LINE__, inp8(controller, ATA_ERR));
            printf("(inp8(controller, ATA_BHR) << 8) | inp8(controller, ATA_BLR);=%d\n", (inp8(controller, ATA_BHR) << 8) | inp8(controller, ATA_BLR));
            return 2;
        }

        byte irr = inp8(controller, ATA_IRR);

        if (((status & BIT_DRQ) != 0) && ((irr & BIT_IO) == 0) && ((irr & BIT_CD) != 0)) break;
    }

    if (i == ATA_TIMEOUT) return 3;

    outp16(controller, (word*)command->packet, 6);

    for (i = 0; i <= ATA_TIMEOUT; i++)
    {
        byte status = inp8(controller, ATA_STR);
        byte irr    = inp8(controller, ATA_IRR);

        if ((status & BIT_BSY) != 0) continue;

        if ((status & BIT_CHK) != 0)
        {
            printf("%s:%d error=%x\n", __FILE__, __LINE__, inp8(controller, ATA_ERR));
            requestSense(controller);
            return 4;
        }

        if (((irr & BIT_IO) != 0) && ((irr & BIT_CD) == 0) && ((status & BIT_DRQ) != 0))
        {
            /* read */
            controller->selectedDevice->dataTransferSize = (inp8(controller, ATA_BHR) << 8) | inp8(controller, ATA_BLR);
            inp16(controller, (word*)buffer, (controller->selectedDevice->dataTransferSize + 1) / 2);

        }

        if (((irr & BIT_IO) == 0) && ((irr & BIT_CD) == 0) && ((status & BIT_DRQ) != 0))
        {
            /* write */
            controller->selectedDevice->dataTransferSize = (inp8(controller, ATA_BHR) << 8) | inp8(controller, ATA_BLR);
            outp16(controller, (word*)buffer, (controller->selectedDevice->dataTransferSize + 1) / 2);
        }

        if (((irr & BIT_IO) != 0) && ((irr & BIT_CD) != 0) && ((status & BIT_DRQ) == 0))
        {
            break;
        }
    }

    inp8(controller, ATA_STR);

    if (i == ATA_TIMEOUT) return 5;
    return 0;
}

int IDEDriver::readATAPI(IDEController* controller, dword lba, void* buffer, int size)
{
    ATAPICommand command;
    memset(&command, 0, sizeof(command));

    /* sector count */
    int count = (size + 2048 - 1) / 2048; // Mm...

    command.feature   = 0;
    command.device    = controller->selectedDevice->deviceNo;
    command.packet[0] = 0x28;
    command.packet[2] = (lba >>  24) & 0xff;
    command.packet[3] = (lba >>  16) & 0xff;
    command.packet[4] = (lba >>   8) & 0xff;
    command.packet[5] = (lba       ) & 0xff;
    command.packet[7] = (count >> 8) & 0xff;
    command.packet[8] = (count     ) & 0xff;
    controller->selectedDevice->dataTransferSize = 0;

    return sendPacketCommand(controller, &command, 2048, buffer);
}

dword IDEDriver::requestSense(IDEController* controller)
{
    byte buffer[512];

    ATAPICommand command;
    memset(&command, 0, sizeof(command));

    command.feature   = 0;
    command.device    = controller->selectedDevice->deviceNo;
    command.packet[0] = 0x03;
    command.packet[4] = 18;

    controller->selectedDevice->dataTransferSize = 0;

    sendPacketCommand(controller, &command, 2048, buffer);

    printf("buffer[0]=%d\n", buffer[0]);
    printf("buffer[1]=%d\n", buffer[1]);
    printf("buffer[2]=%d\n", buffer[2]);
    printf("buffer[3]=%d\n", buffer[3]);
    printf("buffer[4]=%d\n", buffer[4]);
    printf("buffer[5]=%d\n", buffer[5]);
    printf("buffer[6]=%d\n", buffer[6]);
    printf("buffer[7]=%d\n", buffer[7]);
    printf("buffer[8]=%d\n", buffer[8]);
    printf("buffer[9]=%d\n", buffer[9]);
    printf("buffer[10]=%d\n", buffer[10]);
    printf("buffer[11]=%d\n", buffer[11]);
    printf("buffer[12]=%d\n", buffer[12]);
    printf("buffer[13]=%d\n", buffer[13]);
    printf("buffer[14]=%d\n", buffer[14]);
    printf("buffer[15]=%d\n", buffer[15]);

    return 0;
}


int IDEDriver::read(dword lba, void* buffer, int size)
{
    if (this->whichController == NULL) return 1;

    if (this->whichController->selectedDevice->type == DEVICE_ATAPI)
    {
        return readATAPI(this->whichController, lba, buffer, size);
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

void IDEDriver::printDebug()
{
    printDebug(&this->controllers[PRIMARY]  , MASTER);
    printDebug(&this->controllers[PRIMARY]  , SLAVE);
    printDebug(&this->controllers[SECONDARY], MASTER);
    printDebug(&this->controllers[SECONDARY], SLAVE);
}

void IDEDriver::printDebug(IDEController* controller, int deviceNo)
{
    IDEDevice* device = &controller->devices[deviceNo];
    printf("device type=%d,%x %s\n", device->type, device->typeDetail, (const char*)device->name);
}

void IDEDriver::resetAndIdentify(IDEController* controller)
{
    /* software reset */
    outp8(controller, ATA_DCR, 0x06);
    sleep(5);

    /* no interrupt */
    outp8(controller, ATA_DCR, 0x02);
    sleep(5);

    identify(controller, MASTER);
    identify(controller, SLAVE);
    identifyDetail(controller, MASTER);
    identifyDetail(controller, SLAVE);
}

void IDEDriver::identifyDetail(IDEController* controller, int deviceNo)
{
    ATACommand com;
    IDEDevice* device = &controller->devices[deviceNo];
    device->deviceNo = deviceNo;

    if (device->type != DEVICE_ATA && device->type != DEVICE_ATAPI)
    {
        device->name = "none";
        return;
    }

    /* feature, sector count, sector number, cylinder is 0 */
    memset(&com, 0, sizeof(ATACommand));
    com.device = DEV_HEAD_OBS | (deviceNo << 4);

    if (device->type == DEVICE_ATA)
    {
        com.drdyCheck = 1;
        com.command = 0xec; /* Identify device */
    }
    else
    {
        com.drdyCheck = 0;
        com.command = 0xa1; /* Identify Packet device */
    }

    /* command execute */
    word buffer[256];
    memset(buffer, 0, sizeof(buffer));
    int result = sendPioDataInCommand(controller, &com, 1, buffer);
    if (result == 0)
    {
        for (int i = 0; i < 256; i++)
        {
            word value = buffer[i];
            buffer[i] = ((value >> 8) & 0xff) | ((value << 8) & 0xff00);
        }

        if (device->type == DEVICE_ATA)
        {
            device->name = CString((const char*)((byte*)buffer + 54), 40);
            device->typeDetail = -1;
        }
        else if (device->type == DEVICE_ATAPI)
        {
            device->name = CString((const char*)((byte*)buffer + 54), 40);
            device->typeDetail = buffer[0] & 0x1f;
        }
        else
        {
            device->name = "Unknwon device";
            device->typeDetail = -1;
        }
    }
    else
    {
        device->name = "Unknwon device";
        device->typeDetail = -1;
    }
}

bool IDEDriver::selectDevice(IDEController* controller, int deviceNo)
{
    if (!waitBusyAndDataRequestBothClear(controller)) return false;

    /* select device */
    outp8(controller, ATA_DHR, deviceNo << 4);
    sleep(10);

    if (!waitBusyAndDataRequestBothClear(controller)) return false;

    return true;
}

int IDEDriver::sendPioDataInCommand(IDEController* controller, ATACommand* command, word count, void* buf)
{
    word* p = (word*)buf;

    if (!selectDevice(controller, command->device)) return 1;

    outp8(controller, ATA_DCR, 0x02);                  /* not use interrupt */
    outp8(controller, ATA_FTR, command->feature);      /* feature           */
    outp8(controller, ATA_SCR, command->sectorCount);  /* sector count      */
    outp8(controller, ATA_SNR, command->sectorNumber); /* sector number     */
    outp8(controller, ATA_CLR, command->cylinderLow);  /* cylinderLow       */
    outp8(controller, ATA_CHR, command->cylinderHigh); /* cylinderHigh      */

    /* drdy check */
    if (command->drdyCheck && !waitDrdySet(controller)) return 2;

    outp8(controller, ATA_CMR, command->command);      /* command           */
    sleep(10);

    /* read atlternate status once */
    inp8(controller, ATA_ASR);

    /* read */
    for (int i = 0; i < count; i++, p+=256)
    {
        if (!waitBusyClear(controller)) return 3;

        byte status = inp8(controller, ATA_STR);

        /* command error */
        if ((status & BIT_ERR) != 0) return 4;

        /* data not ready */
        if ((status & BIT_DRQ) == 0) return 5;

        /* data read */
        inp16(controller, p, 256);
    }

    inp8(controller, ATA_ASR);
    byte status = inp8(controller, ATA_STR);

    if (status & BIT_ERR)
    {
        byte error = inp8(controller, ATA_ERR);
        return 0x1000 | error;
    }

    return 0;
}

void IDEDriver::inp16(IDEController* controller, word* data, int length)
{
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

void IDEDriver::identify(IDEController* controller, int deviceNo)
{
    IDEDevice* device = &controller->devices[deviceNo];

    int i;
    for (i = 0; i < RETRY_MAX; i++)
    {
        /* device select */
        outp8(controller, ATA_DHR, deviceNo << 4);
        sleep(10);

        /* device not found? */
        byte status = inp8(controller, ATA_STR);
        if (status == 0xff)
        {
            device->type = DEVICE_NONE;
            return;
        }

        /* device not found? */
        if (!waitBusyClear(controller))
        {
            device->type = DEVICE_NONE;
            return;
        }

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

        /* read data head register. selection OK ? */
        byte result = inp8(controller, ATA_DHR);
        if ((result & (deviceNo << 4)) == (deviceNo << 4))
        {
            byte high = inp8(controller, ATA_CHR);
            byte low  = inp8(controller, ATA_CLR);
            device->type = judgeDeviceType(high, low);
            return;
        }
    }

    if (i == RETRY_MAX)
    {
        device->type = DEVICE_UNKNOWN;
    }
    return;
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

int IDEDriver::judgeDeviceType(byte high, byte low)
{
    int type;
    int s = (high << 8 | low);

    switch(s)
    {
    case 0xEB14:
        type = DEVICE_ATAPI;
        break;
    case 0:
        type = DEVICE_ATA;
        break;
    default:
        type = DEVICE_NONE;
        break;
    }

    return type;
}

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
