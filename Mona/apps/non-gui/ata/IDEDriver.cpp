/*!
    \file  IDEDriver.cpp
    \brief IDEDriver

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2004/07/04 update:$Date$
*/

#include "IDEDriver.h"
#include <monapi.h>

/*----------------------------------------------------------------------
    IDEDriver
----------------------------------------------------------------------*/
IDEDriver::IDEDriver()
{
    this->controllers[0].registers[ATA_DTR] = 0x1f0;
    this->controllers[0].registers[ATA_ERR] = 0x1f1;
    this->controllers[0].registers[ATA_SCR] = 0x1f2;
    this->controllers[0].registers[ATA_SNR] = 0x1f3;
    this->controllers[0].registers[ATA_CLR] = 0x1f4;
    this->controllers[0].registers[ATA_CHR] = 0x1f5;
    this->controllers[0].registers[ATA_DHR] = 0x1f6;
    this->controllers[0].registers[ATA_STR] = 0x1f7;
    this->controllers[0].registers[ATA_ASR] = 0x3f6;
    this->controllers[0].selectedDevice = -1;

    this->controllers[1].registers[ATA_DTR] = 0x170;
    this->controllers[1].registers[ATA_ERR] = 0x171;
    this->controllers[1].registers[ATA_SCR] = 0x172;
    this->controllers[1].registers[ATA_SNR] = 0x173;
    this->controllers[1].registers[ATA_CLR] = 0x174;
    this->controllers[1].registers[ATA_CHR] = 0x175;
    this->controllers[1].registers[ATA_DHR] = 0x176;
    this->controllers[1].registers[ATA_STR] = 0x177;
    this->controllers[1].registers[ATA_ASR] = 0x376;
    this->controllers[1].selectedDevice = -1;

    /* soft reset */
    outp8(&controllers[0], ATA_DCR, 0x06);
    sleep(5);

    /* disable interrupt */
    outp8(&controllers[0], ATA_DCR, 0x02);
    sleep(5);

    /* soft reset */
    outp8(&controllers[1], ATA_DCR, 0x06);
    sleep(5);

    /* disable interrupt */
    outp8(&controllers[1], ATA_DCR, 0x02);
    sleep(5);

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            deviceType[i + j * 2] = judgeDevice(&(this->controllers[i]), j);
        }
    }


//    setDevice(PRIMARY, MASTER);‚¢‚ç‚È‚¢H
}

IDEDriver::~IDEDriver()
{
}

int IDEDriver::open()
{
    return 0;
}

int IDEDriver::close()
{
    return 0;
}

int IDEDriver::read(dword lba, void* buffer, int size)
{
    if (this->whichController->selectedDeviceType == DEVICE_ATAPI)
    {
        return readATAPI(this->whichController, lba, buffer, size);
    }
    else if (this->whichController->selectedDeviceType == DEVICE_ATA)
    {
        return readATA(whichController, whichController->selectedDevice, lba, (size + 512 - 1) / 512, buffer);
    }
    else
    {
        return 0;
    }
}

int IDEDriver::sendPacketCommand(IDEController* controller, ATAPICommand* command, word limit, void* buffer)
{
    outp8(controller, ATA_DCR, 0xa);  /* no interrupt */

    if (controller->selectedDevice != command->device)
    {
        if (!selectDevice(controller, command->device)) return 1;
    }
    else
    {
        outp8(controller, ATA_DHR, 0x40 | 0xa0 | (controller->selectedDevice << 4));
    }

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
            return 4;
        }

        if (((irr & BIT_IO) != 0) && ((irr & BIT_CD) == 0) && ((status & BIT_DRQ) != 0))
        {
            /* read */
            controller->dataTransferSize = (inp8(controller, ATA_BHR) << 8) | inp8(controller, ATA_BLR);
            inp16(controller, (word*)buffer, (controller->dataTransferSize + 1) / 2);

        }

        if (((irr & BIT_IO) == 0) && ((irr & BIT_CD) == 0) && ((status & BIT_DRQ) != 0))
        {
            /* write */
            controller->dataTransferSize = (inp8(controller, ATA_BHR) << 8) | inp8(controller, ATA_BLR);
            outp16(controller, (word*)buffer, (controller->dataTransferSize + 1) / 2);
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

bool IDEDriver::selectDevice(IDEController* controller, int device)
{
    dword i;

    if (controller->selectedDevice == device) return true;

    for (i = 0; i < ATA_TIMEOUT; i++)
    {
        byte data = inp8(controller, ATA_ASR);
        if ((data & BIT_BSY) == 0 && (data & BIT_DRQ) == 0) break;
    }

    /* time out */
    if (i == ATA_TIMEOUT) return false;

    /* select device */
    outp8(controller, ATA_DHR, 0xa0 | 0x40 | (device << 4));
    sleep(1);

    for (i = 0; i < ATA_TIMEOUT; i++)
    {
        byte data = inp8(controller, ATA_ASR);
        if ((data & BIT_BSY) == 0 && (data & BIT_DRQ) == 0) break;
    }

    /* time out */
    if (i == ATA_TIMEOUT) return false;

    controller->selectedDevice = device;
    return true;
}

int IDEDriver::readATAPI(IDEController* controller, dword lba, void* buffer, int size)
{
    ATAPICommand command;
    memset(&command, 0, sizeof(command));

    /* sector count */
    int count = (size + CD_SECTOR_SIZE - 1) / CD_SECTOR_SIZE; // Mm...

    command.feature   = 0;
    command.device    = controller->selectedDevice;
    command.packet[0] = 0x28;
    command.packet[2] = (lba >>  24) & 0xff;
    command.packet[3] = (lba >>  16) & 0xff;
    command.packet[4] = (lba >>   8) & 0xff;
    command.packet[5] = (lba       ) & 0xff;
    command.packet[7] = (count >> 8) & 0xff;
    command.packet[8] = (count     ) & 0xff;
    controller->dataTransferSize = 0;

    return sendPacketCommand(controller, &command, CD_SECTOR_SIZE, buffer);
}

int IDEDriver::write(dword lba, void* buffer, int size)
{
    printf("IDEDriver#write not implemented\n");
    return 0;
}

int IDEDriver::ioctl(void* p)
{
    printf("IDEDriver#ioctl not implemented\n");
    return 0;
}

void IDEDriver::outp8(IDEController* controller, int reg, byte value)
{
    ::outp8(controller->registers[reg], value);
}

byte IDEDriver::inp8(IDEController* controller, int reg)
{
    return ::inp8(controller->registers[reg]);
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

word IDEDriver::inp16(IDEController* controller, int reg)
{
    return ::inp16(controller->registers[reg]);
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

int IDEDriver::getDeviceType(int controller, int device)
{
    if (controller != PRIMARY && controller != SECONDARY)  return DEVICE_NON;
    if (device != MASTER && device != SLAVE) return DEVICE_NON;

    return this->deviceType[controller + device * 2];
}

word IDEDriver::getSignature(IDEController* ide, int device)
{
    dword l;
    byte c;
    byte c1 = 0xff; /* unknown signature */
    byte c2 = 0xff; /* unknown signature */
    bool timeout;

    for (l = 0; l < RETRY_MAX; l++)
    {
        /* select device */
        outp8(ide, ATA_DHR, 0xa0 | 0x40 | (device << 4));
        sleep(10);

        c = inp8(ide, ATA_STR);
        if (c == 0xff) break;

        timeout = !waitBusyClear(ide);
        if (timeout) break;

        c = inp8(ide, ATA_ERR);
        if ((c & 0x7f) != 1) break;

        c = inp8(ide, ATA_DHR);
        if ((c & (device << 4)) == (device << 4))
        {
            c1 = inp8(ide, ATA_CLR);
            c2 = inp8(ide, ATA_CHR);
            break;
        }
    }

    return c1 | (c2 << 8);
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

// can call once why?
int IDEDriver::judgeDevice(IDEController* controller, int device)
{
    dword i;
    char buf[1024];
    int type;
    word sigature = getSignature(controller, device);

    switch(sigature)
    {
    case 0xEB14:
        type = DEVICE_ATAPI;
        break;
    case 0:
        type = DEVICE_ATA;
        break;
    default:
        return DEVICE_NON;
    }

    /* unknown device */
    if (!waitBusyClear(controller)) return DEVICE_UNKNOWN;

    for (i = 0; i < RETRY_MAX; i++)
    {
        /* try idetify 2 times */
        sleep(5);
        int id1 = identify(controller, device, type, buf);
        sleep(5);
        int id2 = identify(controller, device, type, buf);

        /* retry */
        if (id1 != id2) continue;
        if (id1 == 3 || id1 == 4) continue;

        if (id1 == 0) return type;
    }

    return DEVICE_UNKNOWN;
}

int IDEDriver::identify(IDEController* controller, int device, int type, void* buf)
{
    ATACommand com;
    int result;

    /* feature, sector count, sector number, cylinder is 0 */
    memset(&com, 0, sizeof(ATACommand));
    com.device = DEV_HEAD_OBS | (device << 4);

    if (type == DEVICE_ATA)
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
    result = sendPioDataInCommand(controller, &com, 1, buf);

    if (result == -3) return 1; /* device not exist */
    else if (result > 0 && (result & BIT_ABRT) != 0) return 2; /* abort */
    else if (result > 0) return 3; /* error */
    else if (result < 0) return 4; /* timeout or device select error */

    return 0;
}

int IDEDriver::sendPioDataInCommand(IDEController* controller, ATACommand* command, word count, void* buf)
{
    word* p;

    p = (word*)buf;

    /* select deviece */
    if (controller->selectedDevice != command->device)
    {
        if (!selectDevice(controller, command->device)) return 1;
    }
    else
    {
        outp8(controller, ATA_DHR, command->device >> 4);
    }

    outp8(controller, ATA_DCR, 0x02);                  /* not use interrupt */
    outp8(controller, ATA_FTR, command->feature);      /* feature           */
    outp8(controller, ATA_SCR, command->sectorCount);  /* sector count      */
    outp8(controller, ATA_SNR, command->sectorNumber); /* sector number     */
    outp8(controller, ATA_CLR, command->cylinderLow);  /* cylinderLow       */
    outp8(controller, ATA_CHR, command->cylinderHigh); /* cylinderHigh      */

    /* drdy check */
    if (command->drdyCheck && !waitDrdySet(controller)) return 2;

    outp8(controller, ATA_CMR, command->command);      /* command           */
    sleep(1);

    /* read atlternate status once */
    inp8(controller, ATA_ASR);

    /* read */
    for (int i = 0; i < count; i++, p+=256)
    {
        if (!waitBusyClear(controller)) return -1;

        byte status = inp8(controller, ATA_STR);

        /* command error */
        if ((status & BIT_ERR) != 0) return -2;

        /* data not ready */
        if ((status & BIT_DRQ) == 0) return -3;

        /* data read */
        read(controller, 256, p);
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

void IDEDriver::read(IDEController* controller, word length, void* buf)
{
    word* p = (word*)buf;

    if (p == NULL)
    {
        for (int i = 0; i < length; i++)
        {
            inp16(controller, ATA_DTR);
        }
    }
    else
    {
        for (int i = 0; i < length; i++, p++)
        {
            *p = inp16(controller, ATA_DTR);
        }
    }
}

int IDEDriver::setDevice(int controller, int device)
{
    if (controller != PRIMARY && controller != SECONDARY)  return 1;
    if (device != MASTER && device != SLAVE) return 2;

    whichController = &controllers[controller];

    whichController->selectedDevice = device;
    whichController->selectedDeviceType = getDeviceType(controller, device);

    selectDevice(whichController, device);

    return 0;
}

int IDEDriver::readATA(IDEController* controller, int device, dword lba, byte blocknum, void* buff)
{
    ATACommand com;
    word cylinder;

    /* feature, sector count, sector number, cylinder is 0 */
    memset(&com, 0, sizeof(ATACommand));

    com.sectorCount  = blocknum;
    com.sectorNumber = lba & 0xff;
    cylinder = (lba >> 8) & 0xffff;

    com.cylinderLow = cylinder & 0xff;
    com.cylinderHigh = (cylinder) >> 8 & 0xff;
    com.device = DEV_HEAD_OBS | LBA_FLG | (device << 4) | ((lba >> 24) & 0xf);
    com.command = 0x20;
    com.drdyCheck = 1;

    return sendPioDataInCommand(controller, &com, blocknum, buff);
}
