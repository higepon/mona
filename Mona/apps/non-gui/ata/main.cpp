#include <monapi.h>

using namespace MonAPI;

#define ATA_DTR     0
#define ATA_ERR     1
#define ATA_FTR     1
#define ATA_SCR     2
#define ATA_IRR     2
#define ATA_SNR     3
#define ATA_CLR     4
#define ATA_BLR     4
#define ATA_CHR     5
#define ATA_BHR     5
#define ATA_DHR     6
#define ATA_STR     7
#define ATA_CMR     7
#define ATA_ASR     8
#define ATA_DCR     8

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


#define ATA_TIMEOUT             10000000L

#define BIT_BSY         0x80    /* Status reg. bit7 */
#define BIT_DRDY        0x40    /* Status reg. bit6 */
#define BIT_DRQ         8               /* Status reg. bit3 */
#define BIT_ABRT        4               /* Status reg. bit2 */
#define BIT_CHK         1               /* Status reg. bit0 (ATAPI) */
#define BIT_ERR         1               /* Status reg. bit0 (ATA) */
#define BIT_IO          2               /* Interrup Reason reg. bit1 */
#define BIT_CD          1               /* Interrup Reason reg. bit0 */

#define RETRY_MAX               8

IDEController ide[2];

void outp8(IDEController* ide, int reg, byte value)
{
    outp8(ide->registers[reg], value);
}

byte inp8(IDEController* ide, int reg)
{
    return inp8(ide->registers[reg]);
}

word inp16(IDEController* ide, int reg)
{
    return inp16(ide->registers[reg]);
}


bool waitBusyClear(IDEController* ide)
{
    dword i;
    for (i = 0; i < ATA_TIMEOUT; i++)
    {
        byte status = inp8(ide, ATA_ASR);
        if ((status & BIT_BSY) == 0) break;
    }
    return (i != ATA_TIMEOUT);
}


bool WaitDrdySet(IDEController* ide)
{
    dword i;

    for (i = 0; i < ATA_TIMEOUT; i++)
    {
        byte status = inp8(ide, ATA_ASR);
        if (status & BIT_DRDY) break;
    }

    return (i != ATA_TIMEOUT);
}


bool SelectDevice(IDEController* ide, int device)
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

word getSignature(IDEController* ide, int device)
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

#if 0
        c = inp8(ide, ATA_ERR);
        if ((c & 0x7f) == 1) {printf("here");break;} /* device broken */
#endif

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


void DataRead(IDEController* ide, word length, void* buf)
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


int SendPioDataCommand(IDEController* ide, ATACommand* command, word count, void* buf)
{
    word* p;

    p = (word*)buf;

    /* select deviece */
    if (ide->selectedDevice != command->device)
    {
        if (!SelectDevice(ide, command->device)) return 1;
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
    if (command->drdyCheck && !WaitDrdySet(ide)) return 2;

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
        DataRead(ide, 256, p);
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


enum DeviceType
{
    DEVICE_UNKNOWN,
    DEVICE_ATA,
    DEVICE_ATAPI
};



#define DEV_HEAD_OBS    0xa0

int IDE_Identifye(IDEController* ide, int device, DeviceType type, void* buf)
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
    result = SendPioDataCommand(ide, &com, 1, buf);

    if (result > -3) return 1; /* device not exist */
    else if (result > 0 && (result & BIT_ABRT) != 0) return 2; /* abort */
    else if (result > 0) return 3; /* error */
    else if (result < 0) return 4; /* timeout or device select error */

    return 0;
}

int MonaMain(List<char*>* pekoe)
{
    /* use io */
    syscall_get_io();

    ide[0].registers[ATA_DTR] = 0x1f0;
    ide[0].registers[ATA_ERR] = 0x1f1;
    ide[0].registers[ATA_SCR] = 0x1f2;
    ide[0].registers[ATA_SNR] = 0x1f3;
    ide[0].registers[ATA_CLR] = 0x1f4;
    ide[0].registers[ATA_CHR] = 0x1f5;
    ide[0].registers[ATA_DHR] = 0x1f6;
    ide[0].registers[ATA_STR] = 0x1f7;
    ide[0].registers[ATA_ASR] = 0x3f6;
    ide[0].selectedDevice = -1;

    ide[1].registers[ATA_DTR] = 0x170;
    ide[1].registers[ATA_ERR] = 0x171;
    ide[1].registers[ATA_SCR] = 0x172;
    ide[1].registers[ATA_SNR] = 0x173;
    ide[1].registers[ATA_CLR] = 0x174;
    ide[1].registers[ATA_CHR] = 0x175;
    ide[1].registers[ATA_DHR] = 0x176;
    ide[1].registers[ATA_STR] = 0x177;
    ide[1].registers[ATA_ASR] = 0x376;
    ide[1].selectedDevice = -1;

    /* soft reset */
    outp8(&ide[0], ATA_DCR, 0x06);
    sleep(5);

    /* disable interrupt */
    outp8(&ide[0], ATA_DCR, 0x02);
    sleep(5);

    char buf[1024];

    printf("signature=%x\n", getSignature(&ide[0], 0));
    printf("signature=%x\n", getSignature(&ide[0], 1));
    printf("signature=%x\n", getSignature(&ide[1], 0));
    printf("signature=%x\n", getSignature(&ide[1], 1));

    printf("identify=%x\n", IDE_Identifye(&ide[0], 0, DEVICE_ATA, buf));

    for (int i = 0; i < 1024; i++)
    {
        printf("%c", buf[i]);
    }

    return 0;
}
