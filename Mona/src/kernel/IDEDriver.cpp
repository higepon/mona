
#include "global.h"
#include "io.h"

#define PIDE_DEVCTRL    0x3f6   // デバイスコントロール

#define MAX_COUNT   10
#define SEC_SIZ     0x200   // 512 bytes / sec

#define STS_DRQ     0x08
#define STS_ERR     0x01

#define CTRL_SRST   0x04
#define CTRL_IDIS   0x02

#define IDE_IDENT   0xec
#define IDE_SETFEA  0xef

// Primary IDE
#define PIDE_DATA       0x1f0   // データレジスタ(R/W)
#define PIDE_ERR        0x1f1   // エラーレジスタ(R/O)
#define PIDE_FEA        0x1f1   // フィーチャーズ(W/O)
#define PIDE_SC         0x1f2   // セクタカウント(R/W)
#define PIDE_SN         0x1f3   // セクタナンバ(R/W)
#define PIDE_CL         0x1f4   // シリンダ・ロウ(R/W)
#define PIDE_CH         0x1f5   // シリンダ・ハイ(R/W)
#define PIDE_DH         0x1f6   // デバイス／ヘッド(R/W)
#define PIDE_STS        0x1f7   // ステータス
#define PIDE_CMD        0x1f7   // コマンド
#define PIDE_ALTSTS     0x3f6   // オルタネートステータス

typedef struct IDEComannd
{
    word command;
    word status;
    byte features;
    word sectorCount;
    byte drive;
    dword lba;
    byte data[1024];
};

void sendComand(IDEComannd* c)
{
    dword lba = c->lba % 0x0fffffff;

    outp8(PIDE_FEA, c->features); /* need ? */

    /* set Drive, lba on lba mode */
    outp8(PIDE_DH, (0xe0 | ((c->drive & 1) << 4)) | ((lba >> 24) & 0x0f));
    outp8(PIDE_CH, ((lba >> 16) & 0xff));
    outp8(PIDE_CL, ((lba >> 8) & 0xff));
    outp8(PIDE_SN, (lba & 0xff));

    outp8(PIDE_SC, (byte)c->sectorCount);
    outp8(PIDE_CMD, (byte)c->command);
}

word dataRead(IDEComannd* c)
{
    word i;
    word count;
    word dat;
    word  *p;

    p = (word*)c->data;
    for (count = 0; count < SEC_SIZ / 2; count++)
    {
        for (i = 0; i < MAX_COUNT; i++)
        {
            dat = inp8(PIDE_STS);
            if (dat & STS_DRQ)
            {
                dat = inp16(PIDE_DATA);
                *p++ = dat;
                break;
            }
        }
        if (i == MAX_COUNT) break;
    }
    return(count);
}

void identPrint(IDEComannd* c)
{
    unsigned int    i;
    unsigned short  *p;
    p = (unsigned short *)c->data;
    p += 27;
    g_console->printf("HDD MODEL    = [");
    for (i=0; i<40; i+=2) {
        g_console->printf("%c%c",(char)(*p >> 8), (char)(*p & 0xff));
        p++;
    }
    g_console->printf("]\n");
    p = (unsigned short *)c->data;
    p += 23;
    g_console->printf("HDD FIRM REV = [");
    for (i=0; i<8; i+=2) {
        g_console->printf("%c%c",(char)(*p >> 8), (char)(*p & 0xff));
        p++;
    }
    g_console->printf("]\n");

    for (int i = 0; i < 1024; i++)
    {
	g_console->printf("%c", c->data[i]);
    }

}


void hddTest()
{
    IDEComannd c;

    /* device control. reset and disable int */
    outp8(PIDE_DEVCTRL,CTRL_SRST | CTRL_IDIS);

    /* device control. normal condition and enable int */
    outp8(PIDE_DEVCTRL,0);

    c.command = IDE_IDENT;
    c.status = 0;
    c.drive = 0;
    c.lba  = 0;
    c.sectorCount = 1;

    sendComand(&c);

    for (dword i = 0; i < 0xfffffff; i++)
    {
        i++;
        i--;
    }

    byte status = inp8(PIDE_STS);

    dataRead(&c);
    identPrint(&c);


}
