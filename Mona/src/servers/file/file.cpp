#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/io.h>
#include "FileServer.h"
#include "IDEDriver.h"
#include "ISO9660FileSystem.h"

using namespace MonAPI;

int currentDrive;
bool cdInitialized;

#define IRQ_PRIMARY   14
#define IRQ_SECONDARY 15

static int irq;
static IDEDriver* cd;
static ISO9660FileSystem* fs;

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

bool initializeCD()
{
    if (cdInitialized) return true;

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
        return false;
    }

    fs = new ISO9660FileSystem(cd);

    if (!fs->Initialize())
    {
        printf("Initialize Error = %d\n", fs->GetLastError());
        delete fs;
        delete cd;
        return false;
    }

    cdInitialized = true;
    return true;
}

int GetCurrentDrive()
{
    return currentDrive;
}

int ChangeDrive(int drive)
{
    if (drive == DRIVE_FD0)
    {
        currentDrive = DRIVE_FD0;
        return drive;
    }
    else if (drive == DRIVE_CD0)
    {
        initializeCD();
        currentDrive = DRIVE_CD0;
        return drive;
    }
    return 0;
}

void initialize()
{
    currentDrive = DRIVE_FD0;
    cdInitialized = false;
}

monapi_cmemoryinfo* ReadFile(const char* path, bool prompt /*= false*/)
{
    if (prompt) printf("%s: Reading %s....", SVR, path);

    int drive        = currentDrive;
    CString filePath = path;
    if (filePath.startsWith("CD0:"))
    {
        filePath = filePath.substring(4, filePath.getLength());
        drive = DRIVE_CD0;
    }
    else if (filePath.startsWith("FD0:"))
    {
        filePath = filePath.substring(4, filePath.getLength());
        drive = DRIVE_FD0;
    }

    if (drive == DRIVE_FD0)
    {
        FileInputStream fis(filePath);
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
    else if (drive == DRIVE_CD0)
    {
        File* file = fs->Open(filePath, 0);

        if (file == NULL)
        {
            if (prompt) printf("read:file not found\n");
            return NULL;
        }

        monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
        if (!monapi_cmemoryinfo_create(ret, file->GetSize() + 1, prompt))
        {
            monapi_cmemoryinfo_delete(ret);
            return NULL;
        }

        file->Seek(0, SEEK_SET);
        file->Read(ret->Data, ret->Size);

        fs->Close(file);

        ret->Data[ret->Size] = 0;
        if (prompt) printf("OK\n");
        return ret;
    }
    return NULL;
}

monapi_cmemoryinfo* ReadDirectory(const char* path, bool prompt /*= false*/)
{
    int drive        = currentDrive;
    CString filePath = path;
    if (filePath.startsWith("CD0:"))
    {
        filePath = filePath.substring(4, filePath.getLength());
        drive = DRIVE_CD0;
    }
    else if (filePath.startsWith("FD0:"))
    {
        filePath = filePath.substring(4, filePath.getLength());
        drive = DRIVE_FD0;
    }

    if (drive == DRIVE_FD0)
    {
        if (syscall_cd(filePath))
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
    else if (drive == DRIVE_CD0)
    {
        _A<FileSystemEntry*> files = fs->GetFileSystemEntries(filePath);

        monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
        int size = files.get_Length();
        if (!monapi_cmemoryinfo_create(ret, sizeof(int) + size * sizeof(monapi_directoryinfo), prompt))
        {
            monapi_cmemoryinfo_delete(ret);
            return NULL;
        }

        memcpy(ret->Data, &size, sizeof(int));
        monapi_directoryinfo* p = (monapi_directoryinfo*)&ret->Data[sizeof(int)];

        FOREACH (FileSystemEntry*, file, files)
        {
            monapi_directoryinfo di;

            di.size = file->GetSize();
            strcpy(di.name, (const char*)file->GetName());
            di.attr = file->IsDirectory() ? ATTRIBUTE_DIRECTORY : 0;
            *p = di;
            p++;
        }
        END_FOREACH
        return ret;
    }
    return NULL;
}
