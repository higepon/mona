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


#define ATA_TIMEOUT             100000

#define BIT_BSY         0x80    /* Status reg. bit7 */
#define BIT_DRDY        0x40    /* Status reg. bit6 */
#define BIT_DRQ         8               /* Status reg. bit3 */
#define BIT_ABRT        4               /* Status reg. bit2 */
#define BIT_CHK         1               /* Status reg. bit0 (ATAPI) */
#define BIT_ERR         1               /* Status reg. bit0 (ATA) */
#define BIT_IO          2               /* Interrup Reason reg. bit1 */
#define BIT_CD          1               /* Interrup Reason reg. bit0 */

#define RETRY_MAX               2

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


bool WaitBusyClear(IDEController* ide)
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

        timeout = !WaitBusyClear(ide);
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
        if (!WaitBusyClear(ide)) return -1;

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
    DEVICE_NON,
    DEVICE_ATA,
    DEVICE_ATAPI
};



#define DEV_HEAD_OBS    0xa0

int IDE_Identify(IDEController* ide, int device, DeviceType type, void* buf)
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

    if (result == -3) return 1; /* device not exist */
    else if (result > 0 && (result & BIT_ABRT) != 0) return 2; /* abort */
    else if (result > 0) return 3; /* error */
    else if (result < 0) return 4; /* timeout or device select error */

    return 0;
}

DeviceType JudgeDevice(IDEController* ide, int device)
{
    dword i;
    char buf[1024];
    DeviceType type;
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
    if (!WaitBusyClear(ide)) return DEVICE_UNKNOWN;

    for (i = 0; i < RETRY_MAX; i++)
    {
        /* try idetify 2 times */
        sleep(5);
        int id1 = IDE_Identify(ide, device, type, buf);
        sleep(5);
        int id2 = IDE_Identify(ide, device, type, buf);

        /* retry */
        if (id1 != id2) continue;
        if (id1 == 3 || id1 == 4) continue;


        if (id1 == 0) return type;
    }

    return DEVICE_UNKNOWN;
}

void PrintDevice(DeviceType type, IDEController* ide, int device)
{
    char buf[1024];
    word* p = (word*)buf;

    memset(&buf, 0, sizeof(buf));

    switch(type)
    {
    case DEVICE_UNKNOWN:
        printf("unknown device\n");
        break;

    case DEVICE_NON:
        printf("non device\n");
        break;

    case DEVICE_ATA:
        printf("ATA device:");
        IDE_Identify(ide, device, DEVICE_ATA, buf);
        printf("[");
        for (int i = 27; i < 47; i++)
        {
            printf("%c", (p[i] & 0xff00)>>8);
            printf("%c", p[i] & 0x00ff);
        }
        printf("]\n");
        break;

    case DEVICE_ATAPI:
        printf("ATAPI device");
        IDE_Identify(ide, device, DEVICE_ATAPI, buf);
        printf("[");
        for (int i = 27; i < 47; i++)
        {
            printf("%c", (p[i] & 0xff00)>>8);
            printf("%c", p[i] & 0x00ff);
        }
        printf("]\n");
        break;
    }
}

#define DEV_HEAD_obs    0xa0    /* Device/Head’¥ì’¥¸’¥¹’¥¿’¤Îobs’¥Ó’¥Ã’¥È’¤Î’ÃÍ(bit8/bit6) */
#define LBA_flg                 0x40    /* LBA or CHS’Áª’Âò’¥Ó’¥Ã’¥È */

int Read(IDEController* ide, int device, dword lba, byte blocknum, void* buff)
{
    ATACommand com;
    word cylinder;
    int result;

    /* feature, sector count, sector number, cylinder is 0 */
    memset(&com, 0, sizeof(ATACommand));

    com.sectorCount  = blocknum;
    com.sectorNumber = lba & 0xff;
    cylinder = (lba >> 8) & 0xffff;

    com.cylinderLow = cylinder & 0xff;
    com.cylinderHigh = (cylinder) >> 8 & 0xff;
    com.device = DEV_HEAD_obs | LBA_flg | (device << 4) | ((lba >> 24) & 0xf);
    com.command = 0x20;
    com.drdyCheck = 1;

    return SendPioDataCommand(ide, &com, blocknum, buff);
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

    DeviceType type;

    type = JudgeDevice(&ide[0], 0);

    PrintDevice(type, &ide[0], 0);
    type = JudgeDevice(&ide[0], 1);
    PrintDevice(type, &ide[0], 1);
    type = JudgeDevice(&ide[1], 0);
    PrintDevice(type, &ide[1], 0);
    type = JudgeDevice(&ide[1], 1);
    PrintDevice(type, &ide[1], 1);

    char buf[512];
    memset(buf, 0, sizeof(buf));

    printf("read result = %d", Read(&ide[0], 0, 1, 1, buf));

    for (int i = 0; i < 100; i++)
    {
        printf("[%x]", buf[i]);
    }

    return 0;
}
