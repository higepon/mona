#include <monapi.h>
#include "IDEDriver.h"
#include <monapi/io.h>

using namespace MonAPI;

#define IRQ_PRIMARY   14
#define IRQ_SECONDARY 15

static int irq;
static IDEDriver* ide;

/*
    課題
    Vmwareでデバイス認識がうまくいかない
    エラーの詳細を配列コピーで渡す
    セクタサイズの取得
    bufferへの大量読み込みをチェック
    idle位いれようよ。
    busy loopがうっとぉしぃ。
    atapi packet のlimitの意味。2048じゃなくて全体サイズ？
    wordサイズの限界を
*/

static void interrupt()
{
    syscall_set_irq_receiver(irq);

    for (MessageInfo msg;;)
    {
        if (MonAPI::Message::receive(&msg) != 0) continue;

        switch (msg.header)
        {
        case MSG_INTERRUPTED:
#if 1
            printf("interrupt irq=%d\n", msg.arg1);
#endif
            ide->protocolInterrupt();
            break;
        default:
            printf("default");
        }
    }
}

/*
  エラー詳細を表示する。
*/
static void printError(const byte* error)
{
    for (int i = 0; i < 18; i++)
    {
        printf("erorr[%d]=%x\n", i, error[i]);
    }
}

int MonaMain(List<char*>* pekoe)
{
    if (pekoe->size() != 1)
    {
        return 0;
    }

    syscall_get_io();

    ide = new IDEDriver();

    /* find CD-ROM */
    int controller, deviceNo;
    if (!ide->findDevice(IDEDriver::DEVICE_ATAPI, 0x05, &controller, &deviceNo))
    {
        printf("CD-ROM Not Found\n");
        delete ide;
        return 1;
    }

    /* set irq number */
    if (controller == IDEDriver::PRIMARY)
    {
        irq = IRQ_PRIMARY;
        outp8(0xa1, inp8(0xa1) & 0xbf);
    }
    else
    {
        irq = IRQ_SECONDARY;
        outp8(0xa1, inp8(0xa1) & 0x7f);
    }

    /* interrupt thread */
    dword id = syscall_mthread_create((dword)interrupt);
    syscall_mthread_join(id);

    if (!ide->selectDevice(controller, deviceNo))
    {
        printf("select device NG error code = %d\n", ide->getLastError());
        delete ide;
        return 1;
    }

    dword lba = atoi(pekoe->get(0));
    byte buffer[2048];
    printf("[read start]\n");
    int readResult = ide->read(lba, buffer, 2048);
    if (readResult != 0)
    {
        printf("Read Error = %d\n", readResult);
        delete ide;
        return 1;
    }
    printf("\n[read end]\n");

    char buf[128];
    dword addr = lba * 2048;
    for (int i = 0; i < 128; i++)
    {
        sprintf(buf, "%08X:", addr + i * 16);
        for (int j = 0; j < 16; j++)
        {
            sprintf(buf + 9 + j * 3, "%02X ", buffer[i * 16 + j]);
        }

        sprintf(buf + 9 + 3 * 16, "| ");
        
        for (int j = 0; j < 16; j++)
        {
            char ch = buffer[i * 16 + j];
            if (ch < ' ') ch = '.';
            buf[9 + 3 * 16 + 2 + j] = ch;
        }
        sprintf(buf + 9 + 3 * 16 + 2 + 16, "\n");
        printf(buf);
    }

    delete ide;
    return 0;
}
