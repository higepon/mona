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
IDEDriver::IDEDriver() : deviceOK(false)
{
    this->ide[0].registers[ATA_DTR] = 0x1f0;
    this->ide[0].registers[ATA_ERR] = 0x1f1;
    this->ide[0].registers[ATA_SCR] = 0x1f2;
    this->ide[0].registers[ATA_SNR] = 0x1f3;
    this->ide[0].registers[ATA_CLR] = 0x1f4;
    this->ide[0].registers[ATA_CHR] = 0x1f5;
    this->ide[0].registers[ATA_DHR] = 0x1f6;
    this->ide[0].registers[ATA_STR] = 0x1f7;
    this->ide[0].registers[ATA_ASR] = 0x3f6;
    this->ide[0].selectedDevice = -1;

    this->ide[1].registers[ATA_DTR] = 0x170;
    this->ide[1].registers[ATA_ERR] = 0x171;
    this->ide[1].registers[ATA_SCR] = 0x172;
    this->ide[1].registers[ATA_SNR] = 0x173;
    this->ide[1].registers[ATA_CLR] = 0x174;
    this->ide[1].registers[ATA_CHR] = 0x175;
    this->ide[1].registers[ATA_DHR] = 0x176;
    this->ide[1].registers[ATA_STR] = 0x177;
    this->ide[1].registers[ATA_ASR] = 0x376;
    this->ide[1].selectedDevice = -1;

    /* soft reset */
    outp8(&ide[0], ATA_DCR, 0x06);
    sleep(5);

    /* disable interrupt */
    outp8(&ide[0], ATA_DCR, 0x02);
    sleep(5);

    setDevice(PRIMARY, MASTER);
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

int IDEDriver::read(dword lba, void* buf, int size)
{
    if (!this->deviceOK) return 10;
    return read(&ide[whichIde], whichDevice, lba, (size + 512 - 1) / 512, buf);
}

int IDEDriver::write(dword lba, void* buf, int size)
{
    printf("IDEDriver#write not implemented\n");
    return 0;
}

int IDEDriver::ioctl(void* p)
{
    printf("IDEDriver#ioctl not implemented\n");
    return 0;
}

int IDEDriver::setDevice(int ide, int device)
{
    if (ide != PRIMARY && ide != SECONDARY)  return 1;
    if (device != MASTER && device != SLAVE) return 2;

    whichIde    = ide;
    whichDevice = device;
    deviceOK    = true;
    return 0;
}

void IDEDriver::outp8(IDEController* ide, int reg, byte value)
{
    ::outp8(ide->registers[reg], value);
}

byte IDEDriver::inp8(IDEController* ide, int reg)
{
    return ::inp8(ide->registers[reg]);
}

word IDEDriver::inp16(IDEController* ide, int reg)
{
    return ::inp16(ide->registers[reg]);
}

bool IDEDriver::waitBusyClear(IDEController* ide)
{
    dword i;
    for (i = 0; i < ATA_TIMEOUT; i++)
    {
        byte status = inp8(ide, ATA_ASR);
        if ((status & BIT_BSY) == 0) break;
    }
    return (i != ATA_TIMEOUT);
}


bool IDEDriver::waitDrdySet(IDEController* ide)
{
    dword i;

    for (i = 0; i < ATA_TIMEOUT; i++)
    {
        byte status = inp8(ide, ATA_ASR);
        if (status & BIT_DRDY) break;
    }

    return (i != ATA_TIMEOUT);
}

bool IDEDriver::selectDevice(IDEController* ide, int device)
{
    dword i;

    if (ide->selectedDevice == device) return true;

    for (i = 0; i < ATA_TIMEOUT; i++)
    {
        byte data = inp8(ide, ATA_ASR);
        if ((data & BIT_BSY) == 0 && (data & BIT_DRQ) == 0) break;
    }

    /* time out */
    if (i == ATA_TIMEOUT) return false;

    /* select device */
    outp8(ide, ATA_DHR, device << 4);
    sleep(1);

    for (i = 0; i < ATA_TIMEOUT; i++)
    {
        byte data = inp8(ide, ATA_ASR);
        if ((data & BIT_BSY) == 0 && (data & BIT_DRQ) == 0) break;
    }

    /* time out */
    if (i == ATA_TIMEOUT) return false;

    ide->selectedDevice = device;
    return true;
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
        outp8(ide, ATA_DHR, device << 4);
        sleep(1);

        c = inp8(ide, ATA_STR);
        if (c == 0xff) break;

        timeout = !waitBusyClear(ide);
        if (timeout) break;

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

void IDEDriver::read(IDEController* ide, word length, void* buf)
{
    word* p = (word*)buf;

    if (p == NULL)
    {
        for (int i = 0; i < length; i++)
        {
            inp16(ide, ATA_DTR);
        }
    }
    else
    {
        for (int i = 0; i < length; i++, p++)
        {
            *p = inp16(ide, ATA_DTR);
        }
    }
}

int IDEDriver::sendPioDataInCommand(IDEController* ide, ATACommand* command, word count, void* buf)
{
    word* p;

    p = (word*)buf;

    /* select deviece */
    if (ide->selectedDevice != command->device)
    {
        if (!selectDevice(ide, command->device)) return 1;
    }
    else
    {
        outp8(ide, ATA_DHR, command->device >> 4);
    }

    outp8(ide, ATA_DCR, 0x02);                  /* not use interrupt */
    outp8(ide, ATA_FTR, command->feature);      /* feature           */
    outp8(ide, ATA_SCR, command->sectorCount);  /* sector count      */
    outp8(ide, ATA_SNR, command->sectorNumber); /* sector number     */
    outp8(ide, ATA_CLR, command->cylinderLow);  /* cylinderLow       */
    outp8(ide, ATA_CHR, command->cylinderHigh); /* cylinderHigh      */

    /* drdy check */
    if (command->drdyCheck && !waitDrdySet(ide)) return 2;

    outp8(ide, ATA_CMR, command->command);      /* command           */
    sleep(1);

    /* read atlternate status once */
    inp8(ide, ATA_ASR);

    /* read */
    for (int i = 0; i < count; i++, p+=256)
    {
        if (!waitBusyClear(ide)) return -1;

        byte status = inp8(ide, ATA_STR);

        /* command error */
        if ((status & BIT_ERR) != 0) return -2;

        /* data not ready */
        if ((status & BIT_DRQ) == 0) return -3;

        /* data read */
        read(ide, 256, p);
    }

    inp8(ide, ATA_ASR);
    byte status = inp8(ide, ATA_STR);

    if (status & BIT_ERR)
    {
        byte error = inp8(ide, ATA_ERR);
        return 0x1000 | error;
    }

    return 0;
}


int IDEDriver::identify(IDEController* ide, int device, int type, void* buf)
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
    result = sendPioDataInCommand(ide, &com, 1, buf);

    if (result == -3) return 1; /* device not exist */
    else if (result > 0 && (result & BIT_ABRT) != 0) return 2; /* abort */
    else if (result > 0) return 3; /* error */
    else if (result < 0) return 4; /* timeout or device select error */

    return 0;
}

int IDEDriver::getDeviceType(int ide, int device)
{
    if (ide != PRIMARY && ide != SECONDARY)  return DEVICE_NON;
    if (device != MASTER && device != SLAVE) return DEVICE_NON;

    return judgeDevice(&(this->ide[ide]), device);
}

int IDEDriver::judgeDevice(IDEController* ide, int device)
{
    dword i;
    char buf[1024];
    int type;
    word sigature = getSignature(ide, device);

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
    if (!waitBusyClear(ide)) return DEVICE_UNKNOWN;

    for (i = 0; i < RETRY_MAX; i++)
    {
        /* try idetify 2 times */
        sleep(5);
        int id1 = identify(ide, device, type, buf);
        sleep(5);
        int id2 = identify(ide, device, type, buf);

        /* retry */
        if (id1 != id2) continue;
        if (id1 == 3 || id1 == 4) continue;

        if (id1 == 0) return type;
    }

    return DEVICE_UNKNOWN;
}

int IDEDriver::read(IDEController* ide, int device, dword lba, byte blocknum, void* buff)
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

    return sendPioDataInCommand(ide, &com, blocknum, buff);
}

