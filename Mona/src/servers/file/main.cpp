#include <monapi.h>
#include <monapi/messages.h>
#include <sys/BinaryTree.h>
#include <monapi/Assert.h>
#include "FileServer.h"
#include "file.h"
#include "bzip2.h"
#include "dtk5s.h"
#include "IDEDriver.h"
#include "ISO9660.h"

using namespace MonAPI;

bool hasCD;               /* public  */
ISO9660* iso;             /* public  */
static IDEDriver* ide;    /* private */
static int irq;           /* private */
#define IRQ_PRIMARY   14
#define IRQ_SECONDARY 15

static void interrupt()
{
    syscall_set_irq_receiver(irq);

    for (MessageInfo msg;;)
    {
        if (MonAPI::Message::receive(&msg) != 0) continue;

        switch (msg.header)
        {
        case MSG_INTERRUPTED:
            ide->protocolInterrupt();
            break;
        default:
            printf("default");
        }
    }
}

void MessageLoop()
{
    BinaryTree<dword> stdoutTree;
    dword self = monapi_get_server_thread_id(ID_FILE_SERVER);

    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg)) continue;

        switch (msg.header)
        {
            case MSG_FILE_READ_DATA:
            {
                monapi_cmemoryinfo* mi = ReadFile(msg.str, msg.arg1);
                if (mi != NULL)
                {
                    Message::reply(&msg, mi->Handle, mi->Size);
                    monapi_cmemoryinfo_delete(mi);
                }
                else
                {
                    Message::reply(&msg);
                }
                break;
            }
            case MSG_FILE_READ_DIRECTORY:
            {
                monapi_cmemoryinfo* mi = ReadDirectory(msg.str, msg.arg1);
                if (mi != NULL)
                {
                    Message::reply(&msg, mi->Handle, mi->Size);
                    monapi_cmemoryinfo_delete(mi);
                }
                else
                {
                    Message::reply(&msg);
                }
                break;
            }
            case MSG_DISPOSE_HANDLE:
                MemoryMap::unmap(msg.arg1);
                Message::reply(&msg);
                break;
            case MSG_FILE_DECOMPRESS_BZ2:
            {
                monapi_cmemoryinfo* mi1 = monapi_cmemoryinfo_new();

                mi1->Handle = msg.arg1;
                mi1->Size   = msg.arg2;
                monapi_cmemoryinfo* mi2 = NULL;

                if (monapi_cmemoryinfo_map(mi1))
                {
                    mi2 = BZ2Decompress(mi1);
                    monapi_cmemoryinfo_dispose(mi1);
                }

                if (mi2 != NULL)
                {
                    Message::reply(&msg, mi2->Handle, mi2->Size);
                    monapi_cmemoryinfo_delete(mi2);
                }
                else
                {
                    Message::reply(&msg);
                }
                monapi_cmemoryinfo_delete(mi1);
                break;
            }
            case MSG_FILE_DECOMPRESS_BZ2_FILE:
            {
                monapi_cmemoryinfo* mi = BZ2DecompressFile(msg.str, msg.arg1 != 0);

                if (mi != NULL)
                {
                    Message::reply(&msg, mi->Handle, mi->Size);
                    delete mi;
                }
                else
                {
                    Message::reply(&msg);
                }
                break;
            }
            case MSG_FILE_DECOMPRESS_ST5:
            {

                monapi_cmemoryinfo* mi1 = monapi_cmemoryinfo_new();
                mi1->Handle = msg.arg1;
                mi1->Size   = msg.arg2;
                monapi_cmemoryinfo* mi2 = NULL;
                if (monapi_cmemoryinfo_map(mi1))
                {
                    mi2 = ST5Decompress(mi1);
                    monapi_cmemoryinfo_dispose(mi1);
                }
                if (mi2 != NULL)
                {
                    Message::reply(&msg, mi2->Handle, mi2->Size);
                    monapi_cmemoryinfo_delete(mi2);
                }
                else
                {
                    Message::reply(&msg);
                }
                monapi_cmemoryinfo_delete(mi1);
                break;
            }
            case MSG_FILE_DECOMPRESS_ST5_FILE:
            {
                monapi_cmemoryinfo* mi = ST5DecompressFile(msg.str, msg.arg1 != 0);
                if (mi != NULL)
                {
                    Message::reply(&msg, mi->Handle, mi->Size);
                    delete mi;
                }
                else
                {
                    Message::reply(&msg);
                }
                break;
            }
            case MSG_STDOUT: /* higepon exp */
            {
                dword stdout = stdoutTree.get(msg.from);

                ASSERT(stdout != NULL);

                MessageInfo reply;

                if (Message::sendReceiveA(&reply, stdout, &msg))
                {
                    /* error, but nothing to do */
                    ASSERT(!"stdout Error");
                }

                Message::reply(&msg);
                break;
            }
            case MSG_STDOUT_REGIST_TO_SERVER:
            {
                dword tid       = msg.arg1;
                dword stdout_id = msg.arg2;
                stdoutTree.add(tid, stdout_id);
                Message::reply(&msg);
                break;
            }
        }
    }
}

static bool cdInitialize()
{
    syscall_get_io();

    ide = new IDEDriver();
    IStorageDevice* cd = ide;

    /* find CD-ROM */
    int controller, deviceNo;
    if (!ide->findDevice(IDEDriver::DEVICE_ATAPI, 0x05, &controller, &deviceNo))
    {
        delete ide;
        return false;
    }

    printf("mounting CD-ROM\n");

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

    outp8(0x21, (inp8(0x21) & 0xFB)); /* IR2 cascade */

    /* interrupt thread */
    dword id = syscall_mthread_create((dword)interrupt);
    syscall_mthread_join(id);

    if (!ide->selectDevice(controller, deviceNo))
    {
        printf("select device NG error code = %d\n", ide->getLastError());
        delete ide;
        return false;
    }

    iso = new ISO9660(cd, "");

    printf("creating directory cache\n");

    if (!iso->Initialize())
    {
        printf("CD-ROM:initialize Error = %d\n", iso->GetLastError());
        delete iso;
        delete cd;
        return false;
    }

    return true;
}

int MonaMain(List<char*>* pekoe)
{
    if (Message::send(Message::lookupMainThread("INIT"), MSG_SERVER_START_OK) != 0)
    {
        printf("%s: INIT error\n", SVR);
        exit(1);
    }

    /* CD-ROM */
    hasCD = cdInitialize();

    MessageLoop();

    return 0;
}

