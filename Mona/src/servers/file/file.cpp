#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/io.h>
#include <monapi/CString.h>
#include "FileServer.h"
//#include "CFile.h"
#include "IDEDriver.h"
#include "ISO9660FileSystem.h"

using namespace MonAPI;

int currentDrive;
bool cdInitialized;

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

void initialize()
{
    currentDrive = FD0;
    cdInitialized = false;
}

bool initializeCD()
{
    syscall_get_io();

    cd = new IDEDriver();

    /* find CD-ROM */
    int controller, deviceNo;
    if (!cd->findDevice(IDEDriver::DEVICE_ATAPI, 0x05, &controller, &deviceNo))
    {
        printf("CD-ROM Not Found\n");
        delete cd;
        return false;
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


}

monapi_cmemoryinfo* ReadFile(const char* file, bool prompt /*= false*/)
{
    if (prompt) printf("%s: Reading %s....", SVR, file);

    if (currentDrive == FD0)
    {
        FileInputStream fis(file);
        if (fis.open() != 0)
        {
            if (prompt) printf("ERROR\n");
            return NULL;
        }

        monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
        if (!monapi_cmemoryinfo_create(ret, fis.getFileSize() + 1, prompt))
        {
            monapi_cmemoryinfo_delete(ret);
            return NULL;
        }

        ret->Size--;
        fis.read(ret->Data, ret->Size);
        fis.close();
        ret->Data[ret->Size] = 0;
        if (prompt) printf("OK\n");
        return ret;
    }
    else if (currentDrive == CD0)
    {

        return NULL;
    }
    return NULL;
}

monapi_cmemoryinfo* ReadDirectory(const char* path, bool prompt /*= false*/)
{
    if (currentDrive == FD0)
    {
        if (syscall_cd(path))
        {
            if (prompt) printf("%s: ERROR: directory not found: %s\n", SVR, path);
            return NULL;
        }
        if (syscall_dir_open())
        {
            if (prompt) printf("%s: ERROR: can not open directory: %s\n", SVR, path);
            return NULL;
        }

        HList<monapi_directoryinfo*> files;
        monapi_directoryinfo di;
        while (syscall_dir_read(di.name, &di.size, &di.attr) == 0)
        {
            files.add(new monapi_directoryinfo(di));
        }
        syscall_dir_close();
        syscall_cd("/");

        monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
        int size = files.size();
        if (!monapi_cmemoryinfo_create(ret, sizeof(int) + size * sizeof(monapi_directoryinfo), prompt))
        {
            monapi_cmemoryinfo_delete(ret);
            while (files.size() > 0) delete files.removeAt(0);
            return NULL;
        }

        memcpy(ret->Data, &size, sizeof(int));
        monapi_directoryinfo* p = (monapi_directoryinfo*)&ret->Data[sizeof(int)];
        while (files.size() > 0)
        {
            monapi_directoryinfo* di2 = files.removeAt(0);
            memcpy(p, di2, sizeof(monapi_directoryinfo));
            delete di2;
            p++;
        }
        return ret;
    }
    else if (currentDrive == CD0)
    {
        return NULL;
    }
    return NULL;
}
