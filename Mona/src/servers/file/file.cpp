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

static byte irq;
static IDEDriver* cd;
static FDCDriver* fd;
static ISO9660FileSystem* isofs;
static FSOperation* fatfs;

void DeviceOn(int drive)
{
    if (drive == DRIVE_FD0)
    {
        fd->motor(ON);
        fd->recalibrate();
        fd->recalibrate();
        fd->recalibrate();
    }
    else if (drive == DRIVE_CD0)
    {
        /* do nothing */
    }
    return;
}

void DeviceOff(int drive)
{
    if (drive == DRIVE_FD0)
    {
        fd->motorAutoOff();
    }
    else if (drive == DRIVE_CD0)
    {
        /* do nothing */
    }
    return;
}

bool InitializeFD()
{
    syscall_get_io();

    monapi_set_irq(6, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(6);

    fd = new FDCDriver();

    DeviceOn(DRIVE_FD0);
    fatfs = new FSOperation();

    if (fatfs == NULL || !(fatfs->initialize((IStorageDevice*)fd)))
    {
        printf("FSOperation::initialize error\n");
        for (;;);
    }

    DeviceOff(DRIVE_FD0);

    return true;
}

bool InitializeCD()
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
    irq = controller == IDEDriver::PRIMARY ? IRQ_PRIMARY : IRQ_SECONDARY;

    monapi_set_irq(irq, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(irq);

    if (!cd->selectDevice(controller, deviceNo))
    {
        printf("select device NG error code = %d\n", cd->getLastError());
        delete cd;
        return false;
    }

    isofs = new ISO9660FileSystem(cd);

    if (!isofs->Initialize())
    {
        printf("Initialize Error = %d\n", isofs->GetLastError());
        delete isofs;
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
        if (!InitializeCD()) return MONA_FAILURE;
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
    DeviceOn(DRIVE_FD0);
    bool result = fatfs->cd(dir);

    DeviceOff(DRIVE_FD0);
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
        if (!isofs->IsExistDirectory(fullPath))
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

void Initialize(bool bootFromCD)
{
    /* current directory */
    currentDirectory.Alloc(2);

    currentDirectory[DRIVE_FD0] = "/";
    currentDirectory[DRIVE_CD0] = "/";

    /* current drive */
    currentDrive = DRIVE_FD0;
    cdInitialized = false;

    if (bootFromCD)
    {
        /* boot from CD */
        ChangeDrive(DRIVE_CD0);
        InitializeCD();
    }
    else
    {
        /* boot from FD */
        InitializeFD();
    }
}

monapi_cmemoryinfo* ReadFileFromISO9660(const char* path, bool prompt)
{
        if (!InitializeCD()) return NULL;

        File* file = isofs->Open(path, 0);

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

        isofs->Close(file);

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


monapi_cmemoryinfo* ReadFileFromFat(const char* path, bool prompt)
{
    CString copyPath = path;

    DeviceOn(DRIVE_FD0);

    if (!fatfs->open((char*)(const char*)copyPath, FILE_OPEN_READ))
    {
        DeviceOff(DRIVE_FD0);

        if (prompt) printf("ERROR=%d\n", fatfs->getErrorNo());
        return NULL;
    }

    dword size = fatfs->size();

    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    if (!monapi_cmemoryinfo_create(ret, size + 1, prompt))
    {
        monapi_cmemoryinfo_delete(ret);
        DeviceOff(DRIVE_FD0);
        return NULL;
    }

    ret->Size--;

    if (!fatfs->read(ret->Data, ret->Size))
    {
        DeviceOff(DRIVE_FD0);
        monapi_cmemoryinfo_delete(ret);
        if (prompt) printf("ERROR=%d\n", fatfs->getErrorNo());
        return NULL;
    }

    fatfs->close();
    DeviceOff(DRIVE_FD0);
    ret->Data[ret->Size] = 0;
    if (prompt) printf("OK\n");
    return ret;
}

CString ChangeDriveAuto(const CString& path)
{
    CString pathWithoutDriveLetter = path;

    if (path.startsWith("CD0:"))
    {
        pathWithoutDriveLetter = path.substring(4, path.getLength());
        ChangeDrive(DRIVE_CD0);
    }
    else if (path.startsWith("FD0:"))
    {
        pathWithoutDriveLetter= path.substring(4, path.getLength());
        ChangeDrive(DRIVE_FD0);
    }

    return pathWithoutDriveLetter;
}

monapi_cmemoryinfo* ReadFile(const char* path, bool prompt /*= false*/)
{
    monapi_cmemoryinfo* result = NULL;

    if (prompt) printf("%s: Reading %s....", SVR, path);

    /* save current drive */
    int saveDrive = currentDrive;

    /* change drive */
    CString pathWithoutDriveLetter = ChangeDriveAuto(path);

    /* read */
    if (GetCurrentDrive() == DRIVE_FD0)
    {
        result = ReadFileFromFat(pathWithoutDriveLetter, prompt);
    }
    else if (GetCurrentDrive() == DRIVE_CD0)
    {
        result =  ReadFileFromISO9660(pathWithoutDriveLetter, prompt);
    }

    /* restore current drive */
    currentDrive = saveDrive;
    return result;
}


monapi_cmemoryinfo* ReadDirectoryFromFat(const char* path, bool prompt)
{
    CString tmp = path;
    if (!fatChangeDirectory((char*)(const char*)tmp))
    {
        if (prompt) printf("%s: ERROR: directory not found: %s\n", SVR, path);
        return NULL;
    }

    DeviceOn(DRIVE_FD0);

    if (!fatfs->openDir())
    {
        if (prompt) printf("%s: ERROR: can not open directory: %s\n", SVR, path);
        DeviceOff(DRIVE_FD0);
        return NULL;
    }

    /* read directory */
    HList<monapi_directoryinfo*> files;
    monapi_directoryinfo di;
    while (fatfs->readDir(di.name, &di.size, &di.attr))
    {
        files.add(new monapi_directoryinfo(di));
    }
    fatfs->closeDir();

    DeviceOff(DRIVE_FD0);

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

monapi_cmemoryinfo* ReadDirectoryFromISO9660(const char* path, bool prompt)
{
    _A<FileSystemEntry*> files = isofs->GetFileSystemEntries(path);

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

monapi_cmemoryinfo* ReadDirectory(const char* path, bool prompt /*= false*/)
{
    monapi_cmemoryinfo* result = NULL;

    /* save current drive */
    int saveDrive = currentDrive;

    /* change drive */
    CString pathWithoutDriveLetter = ChangeDriveAuto(path);

    /* read directory */
    if (GetCurrentDrive() == DRIVE_FD0)
    {
        result = ReadDirectoryFromFat(pathWithoutDriveLetter, prompt);
    }
    else if (GetCurrentDrive() == DRIVE_CD0)
    {
        result = ReadDirectoryFromISO9660(pathWithoutDriveLetter, prompt);
    }

    /* restore current drive */
    currentDrive = saveDrive;
    return result;
}
