#include <monapi.h>
#include "IDEDriver.h"

using namespace MonAPI;

#define IRQ_PRIMARY   14
#define IRQ_SECONDARY 15

static int irq;
static IDEDriver* ide;

/*
    課題
    Vmwareでデバイス認識がうまくいかない
    エラーの詳細を配列コピーで渡す
    上位階層へのエラー通知方法
    セクタサイズの取得
    bufferへの大量読み込みをチェック
    idle位いれようよ。
    busy loopがうっとぉしぃ。
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
//            printf("interrupt irq=%d\n", msg.arg1);
            ide->interrupt();
            break;
        default:
            printf("default");
        }
    }
}


int MonaMain(List<char*>* pekoe)
{
    syscall_get_io();

    ide = new IDEDriver();
    ide->printDebug();

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
        printf("select device NG\n");
        delete ide;
        return 1;
    }

    char* buf = (char*)malloc(6 * 1024 * 1024);
    memset(buf, 0, sizeof(buf));

    printf("read result = %d\n", ide->read(16, buf, 6 * 1024 * 1024));

    FileOutputStream fos("HDDUMP.TXT");

    printf("fileout:open=%d\n", fos.open());

    printf("fileout:read=%d\n", fos.write((byte*)buf        , 512));
    printf("fileout:read=%d\n", fos.write((byte*)(buf + 512), 512));
    printf("fileout:read=%d\n", fos.write((byte*)buf  +1024      , 512));
    printf("fileout:read=%d\n", fos.write((byte*)(buf +1536), 512));

    fos.close();

    free(buf);
    delete ide;

    return 0;
}
