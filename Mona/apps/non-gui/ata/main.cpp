#include <monapi.h>
#include "IDEDriver.h"
#include "ISO9660.h"

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
#if 0
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
    syscall_get_io();

    ide = new IDEDriver();
    IStorageDevice* cd = ide;


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

    ISO9660* iso = new ISO9660(cd, "");

    if (!iso->Initialize())
    {
        printf("Initialize Error = %d\n", iso->GetLastError());
        delete iso;
        delete cd;
        return -1;
    }

    File* file = iso->GetFile("SRC/SERVERS/ELF/MAIN.CPP");
    if (file == NULL)
    {
        printf("file not found\n");
        delete iso;
        delete cd;
        return -1;
    }

    char* buffer = new char[file->GetSize()];
    file->Seek(0, SEEK_SET);
    file->Read(buffer, file->GetSize());

    for (dword i = 0; i < file->GetSize(); i++)
    {
        printf("%c", buffer[i]);
    }
//    printf("[%s] %d bytes %d/%d/%d %d:%d:%d\n", (const char*)file->GetName(), file->GetSize(), file->year, file->month, file->day, file->hour, file->min, file->sec);

    _A<CString> files = iso->GetFileSystemEntries("SRC/SERVERS");

    FOREACH (CString, file, files)
    {
        printf("%s\n", (const char*)file);
    }
    END_FOREACH

    delete file;
    delete buffer;
    delete iso;
    delete cd;

#if 0
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

    char* buf = (char*)malloc(6 * 1024 * 1024);
    memset(buf, 0, sizeof(buf));

    int readResult = ide->read(18, buf, 4096);

    if (readResult != 0)
    {
        byte buffer[18];
        printf("read error result=%d error code = %d\n", ide->getLastError());
        ide->getLastErrorDetail(buffer);
        printError(buffer);
        delete ide;
        return 1;
    }

    for (int i = 0; i < 5; i++)
    {
	printf("[%x]", buf[i]);
    }
    printf("\n");

    for (int i = 2048; i < 2048 +5; i++)
    {
	printf("[%x]", buf[i]);
    }
    printf("\n");

    FileOutputStream fos("HDDUMP.TXT");

    printf("fileout:open=%d\n", fos.open());

    printf("fileout:write=%d\n", fos.write((byte*)buf        , 800 * 1024));

    fos.close();

    free(buf);
    delete ide;
#endif
    return 0;
}
