#include "ISO9660FileSystem.h"
#include "IDEDriver.h"
#include <monapi/io.h>

using namespace MonAPI;

#define IRQ_PRIMARY   14
#define IRQ_SECONDARY 15

static int irq;
static IDEDriver* cd;

static void interrupt()
{
    syscall_set_irq_receiver(irq);

    for (MessageInfo msg;;)
    {
        if (MonAPI::Message::receive(&msg) != 0) continue;

        switch (msg.header)
        {
        case MSG_INTERRUPTED:

            cd->protocolInterrupt();
            break;
        default:
            printf("default");
        }
    }
}


bool read(FileSystem* iso, const char* filePath)
{
    File* file = iso->Open(filePath, 0);

    if (file == NULL)
    {
        printf("read:file not found\n");
        return false;
    }

    char* buffer = new char[file->GetSize()];
    file->Seek(0, SEEK_SET);
    file->Read(buffer, file->GetSize());

    for (dword i = 0; i < file->GetSize(); i++)
    {
        printf("%c", buffer[i]);
    }

    delete [] buffer;

    iso->Close(file);

    return true;
}

int MonaMain(List<char*>* pekoe)
{
    syscall_get_io();

    if (pekoe->size() < 2)
    {
        printf("usage: CDFSREAD comand [option|file|directory|\n");
        printf("command: read, ls, cp\n");
        return 1;
    }

    const char* isoImage = pekoe->get(0);
    const char* command  = pekoe->get(1);
    const char* arg1     = pekoe->get(2);
    const char* arg2     = pekoe->size() >= 4 ? pekoe->get(3) : NULL;

    cd = new IDEDriver();

    /* find CD-ROM */
    int controller, deviceNo;
    if (!cd->findDevice(IDEDriver::DEVICE_ATAPI, 0x05, &controller, &deviceNo))
    {
        printf("CD-ROM Not Found\n");
        delete cd;
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

    if (!cd->selectDevice(controller, deviceNo))
    {
        printf("select device NG error code = %d\n", cd->getLastError());
        delete cd;
        return 1;
    }

    ISO9660FileSystem* fs = new ISO9660FileSystem(cd);

    if (!fs->Initialize())
    {
        printf("Initialize Error = %d\n", fs->GetLastError());
        delete fs;
        delete cd;
        return 1;
    }

    if (strcmp(command, "read") == 0)
    {
        read(fs, arg1);
    }
    else if (strcmp(command, "ls") == 0 && strcmp(arg1, "-l") == 0)
    {
        _A<FileSystemEntry*> files = fs->GetFileSystemEntries(arg2);

        char* month[] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

        FOREACH (FileSystemEntry*, file, files)
        {
            FileDate* date = file->GetCreateDate();

            printf("%d %s %d %d %s%s\n"
                   , file->IsDirectory() ? 0 : file->GetSize()
                   , month[date->GetMonth()]
                   , date->GetDay()
                   , date->GetYear()
                   , (const char*)file->GetName()
                   , file->IsDirectory() ? "/" : "");
        }
        END_FOREACH
    }
    else if (strcmp(command, "ls") == 0)
    {
        _A<FileSystemEntry*> files = fs->GetFileSystemEntries(arg1);

        FOREACH (FileSystemEntry*, file, files)
        {
            printf("%s\n", (const char*)file->GetName());
        }
        END_FOREACH
    }
    else
    {
        printf("command %s not supported\n", command);
    }

    delete fs;
    delete cd;

    return 0;
}
