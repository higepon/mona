#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/io.h>
#include "FileServer.h"
#include "IDEDriver.h"
#include "ISO9660FileSystem.h"
#include "FDCDriver.h"
#include "FSOperation.h"

using namespace MonAPI;

static int currentDrive;
static _A<CString> currentDirectory;
static bool cdInitialized;

#define IRQ_PRIMARY   14
#define IRQ_SECONDARY 15

static int irq;
static IDEDriver* cd;
static FDCDriver* fd;
static ISO9660FileSystem* fs;
static FSOperation* fso;

#if 0
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
#endif

bool fdInitialize()
{
    syscall_get_io();

    monapi_set_irq(6, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(6);

    fd = new FDCDriver();

    fd->motor(true);
    fd->recalibrate();
    fd->recalibrate();

    fso = new FSOperation();

    if (fso == NULL || !(fso->initialize((IStorageDevice*)fd)))
    {
        printf("FSOperation::initialize error\n");
        for (;;);
    }

    return true;
}

bool initializeCD()
{
    if (cdInitialized) return true;

    syscall_get_io();

    cd = new IDEDriver(IRQ_PRIMARY, IRQ_SECONDARY);

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

#if 0
    /* interrupt thread */
    dword id = syscall_mthread_create((dword)interrupt);
    syscall_mthread_join(id);
#endif

    syscall_set_irq_receiver(irq);

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
        return MONA_SUCCESS;
    }
    else if (drive == DRIVE_CD0)
    {
        if (!initializeCD()) return MONA_FAILURE;
        currentDrive = DRIVE_CD0;
        return MONA_SUCCESS;
    }
    return MONA_FAILURE;
}

const char* GetCurrentDirectory()
{
    return (const char*)currentDirectory[currentDrive];
}

bool fatChangeDirectory(char* dir)
{
    fd->motor(ON);
    fd->recalibrate();
    fd->recalibrate();
    fd->recalibrate();

    bool result = fso->cd(dir);

    fd->motorAutoOff();
    return result;
}

int ChangeDirectory(const CString& dir)
{
    CString fullPath = mergeDirectory(currentDirectory[currentDrive], dir);

    if (currentDrive == DRIVE_FD0)
    {
        CString tmp = dir;
        if (!fatChangeDirectory((char*)(const char*)tmp))
        {
            return MONA_FAILURE;
        }
    }
    else if (currentDrive == DRIVE_CD0)
    {
        if (!fs->IsExistDirectory(fullPath))
        {
            return MONA_FAILURE;
        }
    }

    currentDirectory[currentDrive] = fullPath;

    return MONA_SUCCESS;
}

CString getParentDirectory(const CString& dir)
{
    if (dir == NULL || dir == "/") return "/";

    int p = dir.lastIndexOf('/');
    if (p < 1) return "/";

    return dir.substring(0, p);
}

CString mergeDirectory(const CString& dir1, const CString& dir2)
{
    if (dir2.startsWith("/")) return dir2.toUpper();

    CString ret = dir1;
    _A<CString> dirs = dir2.split('/');
    FOREACH (CString, d, dirs)
    {
        if (d == NULL || d == ".") continue;

        if (d == "..")
        {
            ret = getParentDirectory(ret);
        }
        else
        {
            if (ret != "/") ret += '/';
            ret += d.toUpper();
        }
    }
    END_FOREACH
    return ret;
}


void initialize()
{
    /* current directory */
    currentDirectory.Alloc(2);

    currentDirectory[DRIVE_FD0] = "/";
    currentDirectory[DRIVE_CD0] = "/";

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
        if (!initializeCD()) return NULL;
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
        fd->motor(ON);
        fd->recalibrate();
        fd->recalibrate();
        fd->recalibrate();

        CString copyPath = path;

        if (!fso->open((char*)(const char*)copyPath, FILE_OPEN_READ))
        {
            fd->motorAutoOff();

            if (prompt) printf("ERROR=%d\n", fso->getErrorNo());
            return NULL;
        }

        dword size = fso->size();

        monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
        if (!monapi_cmemoryinfo_create(ret, size + 1, prompt))
        {
            monapi_cmemoryinfo_delete(ret);
            fd->motorAutoOff();
            return NULL;
        }

        ret->Size--;

        if (!fso->read(ret->Data, ret->Size))
        {
            fd->motorAutoOff();
            monapi_cmemoryinfo_delete(ret);
            if (prompt) printf("ERROR=%d\n", fso->getErrorNo());
            return NULL;
        }

        fso->close();
        fd->motorAutoOff();
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

        ret->Size--;
        file->Seek(0, SEEK_SET);
        file->Read(ret->Data, ret->Size);

        fs->Close(file);

#if 0
        logprintf("\n\n%s read start\n", (const char*)filePath);
        int size = ret->Size / 16;

        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                char buf[128];
                sprintf(buf, "%2x ", ret->Data[i* 16 + j]);
                logprintf(buf);
            }
            logprintf("   ");
            for (int j = 0; j < 16; j++)
            {
                char buf[128];
                sprintf(buf, "%c", ret->Data[i* 16 + j]);
                logprintf(buf);
            }


            logprintf("\n");
        }

#endif
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
        CString tmp = filePath;
        if (!fatChangeDirectory((char*)(const char*)tmp))
        {
            if (prompt) printf("%s: ERROR: directory not found: %s\n", SVR, path);
            return NULL;
        }

        fd->motor(ON);
        fd->recalibrate();
        fd->recalibrate();
        fd->recalibrate();

        if (!fso->openDir())
        {
            if (prompt) printf("%s: ERROR: can not open directory: %s\n", SVR, path);
            fd->motorAutoOff();
            return NULL;
        }

        HList<monapi_directoryinfo*> files;
        monapi_directoryinfo di;
        while (fso->readDir(di.name, &di.size, &di.attr))
        {
            files.add(new monapi_directoryinfo(di));
        }
        fso->closeDir();
        fd->motorAutoOff();
        tmp = currentDirectory[currentDrive];
        fatChangeDirectory((char*)(const char*)tmp);

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
