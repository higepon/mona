#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/io.h>
#include "FileServer.h"
#include "IDEDriver.h"
#include "ISO9660FileSystem.h"
#include "FDCDriver.h"
#include "FSOperation.h"

using namespace MonAPI;

#define IRQ_PRIMARY   14
#define IRQ_SECONDARY 15

class FileSystemManager
{
public:
    FileSystemManager() {}
    virtual  ~FileSystemManager() {};

    virtual  bool Initialize() = 0;
    virtual  bool ChangeDirectory(const CString& directory) = 0;
    virtual  void DeviceOn()  = 0;
    virtual  void DeviceOff() = 0;

    virtual  monapi_cmemoryinfo* ReadFile(const char* path, bool prompt)      = 0;
    virtual  monapi_cmemoryinfo* ReadDirectory(const char* path, bool prompt) = 0;

    virtual  void SetCurrentDirectory(const CString& directory) = 0;
    virtual  CString GetCurrentDirectory() const                = 0;
};

class ISO9660FileSystemManager : public FileSystemManager
{
public:
    ISO9660FileSystemManager();
    virtual ~ISO9660FileSystemManager();

    bool Initialize();
    bool ChangeDirectory(const CString& directory);
    void DeviceOn();
    void DeviceOff();

    monapi_cmemoryinfo* ReadFile(const char* path, bool prompt);
    monapi_cmemoryinfo* ReadDirectory(const char* path, bool prompt);

    void SetCurrentDirectory(const CString& directory) {this->currentDirectory = directory;}
    CString GetCurrentDirectory() const {return this->currentDirectory;}

protected:
    IDEDriver* cd;
    ISO9660FileSystem* fs;
    CString currentDirectory;
    bool initialized;
};

class FatFileSystemManager : public FileSystemManager
{
public:
    FatFileSystemManager();
    virtual ~FatFileSystemManager();

    bool Initialize();
    bool ChangeDirectory(const CString& directory);
    void DeviceOn();
    void DeviceOff();

    monapi_cmemoryinfo* ReadFile(const char* path, bool prompt);
    monapi_cmemoryinfo* ReadDirectory(const char* path, bool prompt);

    void SetCurrentDirectory(const CString& directory) {this->currentDirectory = directory;}
    CString GetCurrentDirectory() const {return this->currentDirectory;}

protected:
    FDCDriver* fd;
    FSOperation* fs;
    CString currentDirectory;
};

FatFileSystemManager::FatFileSystemManager()
{
}

FatFileSystemManager::~FatFileSystemManager()
{
}

bool FatFileSystemManager::Initialize()
{
    syscall_get_io();

    monapi_set_irq(6, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(6);

    this->fd = new FDCDriver();

    DeviceOn();
    this->fs = new FSOperation();

    if (this->fs == NULL || !(this->fs->initialize((IStorageDevice*)this->fd)))
    {
        printf("FSOperation::initialize error\n");
        for (;;);
    }

    DeviceOff();

    return true;
}

bool FatFileSystemManager::ChangeDirectory(const CString& directory)
{
    CString tmp = directory;
    DeviceOn();
    bool result = fs->cd((char*)(const char*)tmp);
    DeviceOff();
    return result;
}

void FatFileSystemManager::DeviceOn()
{
    this->fd->motor(ON);
    this->fd->recalibrate();
    this->fd->recalibrate();
    this->fd->recalibrate();
    return;
}

void FatFileSystemManager::DeviceOff()
{
    fd->motorAutoOff();
    return;
}

monapi_cmemoryinfo* FatFileSystemManager::ReadFile(const char* path, bool prompt)
{
    CString copyPath = path;

    DeviceOn();

    /* file open */
    if (!this->fs->open((char*)(const char*)copyPath, FILE_OPEN_READ))
    {
        DeviceOff();

        if (prompt) printf("ERROR=%d\n", fs->getErrorNo());
        return NULL;
    }

    dword size = fs->size();

    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    if (!monapi_cmemoryinfo_create(ret, size + 1, prompt))
    {
        monapi_cmemoryinfo_delete(ret);
        DeviceOff();
        return NULL;
    }

    ret->Size--;

    if (!this->fs->read(ret->Data, ret->Size))
    {
        DeviceOff();
        monapi_cmemoryinfo_delete(ret);
        if (prompt) printf("ERROR=%d\n", this->fs->getErrorNo());
        return NULL;
    }

    this->fs->close();
    DeviceOff();
    ret->Data[ret->Size] = 0;
    if (prompt) printf("OK\n");
    return ret;
}

monapi_cmemoryinfo* FatFileSystemManager::ReadDirectory(const char* path, bool prompt)
{
    CString tmp = path;
    if (!ChangeDirectory((char*)(const char*)tmp))
    {
        if (prompt) printf("%s: ERROR: directory not found: %s\n", SVR, path);
        return NULL;
    }

    DeviceOn();

    if (!fs->openDir())
    {
        if (prompt) printf("%s: ERROR: can not open directory: %s\n", SVR, path);
        DeviceOff();
        return NULL;
    }

    /* read directory */
    HList<monapi_directoryinfo*> files;
    monapi_directoryinfo di;
    while (this->fs->readDir(di.name, &di.size, &di.attr))
    {
        files.add(new monapi_directoryinfo(di));
    }
    this->fs->closeDir();

    DeviceOff();

    tmp = GetCurrentDirectory();
    ChangeDirectory((char*)(const char*)tmp);

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

ISO9660FileSystemManager::ISO9660FileSystemManager()
{
    this->initialized = false;
}

ISO9660FileSystemManager::~ISO9660FileSystemManager()
{
}

bool ISO9660FileSystemManager::Initialize()
{
    if (this->initialized) return true;

    /* user mode I/O */
    syscall_get_io();

    /* IDE Driver */
    this->cd = new IDEDriver(IRQ_PRIMARY, IRQ_SECONDARY);

    /* find CD-ROM */
    int controller, deviceNo;
    if (!this->cd->findDevice(IDEDriver::DEVICE_ATAPI, 0x05, &controller, &deviceNo))
    {
        printf("CD-ROM Not Found\n");
        delete this->cd;
        return false;
    }

    /* set irq number */
    byte irq = controller == IDEDriver::PRIMARY ? IRQ_PRIMARY : IRQ_SECONDARY;

    /* enable interrupts */
    monapi_set_irq(irq, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(irq);

    /* CD Select Device */
    if (!this->cd->selectDevice(controller, deviceNo))
    {
        printf("select device NG error code = %d\n", this->cd->getLastError());
        delete this->cd;
        return false;
    }

    this->fs = new ISO9660FileSystem(cd);

    /* initialize ISO9660 FS */
    if (!this->fs->Initialize())
    {
        printf("Initialize Error = %d\n", this->fs->GetLastError());
        delete this->fs;
        delete this->cd;
        return false;
    }

    this->initialized = true;
    return true;
}

bool ISO9660FileSystemManager::ChangeDirectory(const CString& directory)
{
    return fs->IsExistDirectory(directory);
}

void ISO9660FileSystemManager::DeviceOn()
{
    /* do nothing */
    return;
}

void ISO9660FileSystemManager::DeviceOff()
{
    /* do nothing */
    return;
}

monapi_cmemoryinfo* ISO9660FileSystemManager::ReadFile(const char* path, bool prompt)
{
    if (!this->Initialize()) return NULL;

    /* file open */
    File* file = fs->Open(path, 0);

    if (file == NULL)
    {
        if (prompt) printf("read:file not found\n");
        return NULL;
    }

    /* allocate memory */
    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    if (!monapi_cmemoryinfo_create(ret, file->GetSize() + 1, prompt))
    {
        monapi_cmemoryinfo_delete(ret);
        return NULL;
    }

    /* read */
    ret->Size--;
    file->Seek(0, SEEK_SET);
    file->Read(ret->Data, ret->Size);

    fs->Close(file);

    ret->Data[ret->Size] = 0;
    if (prompt) printf("OK\n");
    return ret;
}

monapi_cmemoryinfo* ISO9660FileSystemManager::ReadDirectory(const char* path, bool prompt)
{
    /* read directory */
    _A<FileSystemEntry*> files = this->fs->GetFileSystemEntries(path);

    /* allcoate memory */
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

static FileSystemManager* isofs;
static FileSystemManager* fatfs;
static FileSystemManager* fs;
int currentDrive;

int GetCurrentDrive()
{
    return currentDrive;
}

int ChangeDrive(int drive)
{
    if (drive == DRIVE_FD0)
    {
        currentDrive = DRIVE_FD0;
        fs = fatfs;
        return MONA_SUCCESS;
    }
    else if (drive == DRIVE_CD0)
    {
        if (!isofs->Initialize()) return MONA_FAILURE;
        currentDrive = DRIVE_CD0;
        fs = isofs;
        return MONA_SUCCESS;
    }
    return MONA_FAILURE;
}

const char* GetCurrentDirectory()
{
    return fs->GetCurrentDirectory();
}

int ChangeDirectory(const CString& dir)
{
    CString fullPath = mergeDirectory(fs->GetCurrentDirectory(), dir);

    bool isOK = fs->ChangeDirectory(fullPath);

    if (!isOK) return MONA_FAILURE;

    fs->SetCurrentDirectory(fullPath);

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
    /* current drive */
    currentDrive = DRIVE_FD0;

    isofs = new ISO9660FileSystemManager();
    fatfs = new FatFileSystemManager();
    isofs->SetCurrentDirectory("/");
    fatfs->SetCurrentDirectory("/");

    if (bootFromCD)
    {
        /* boot from CD */
        ChangeDrive(DRIVE_CD0);
        isofs->Initialize();
    }
    else
    {
        /* boot from FD */
        fatfs->Initialize();
    }
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
    result = fs->ReadFile(pathWithoutDriveLetter, prompt);

    /* restore current drive */
    currentDrive = saveDrive;
    return result;
}


monapi_cmemoryinfo* ReadDirectory(const char* path, bool prompt /*= false*/)
{
    monapi_cmemoryinfo* result = NULL;

    /* save current drive */
    int saveDrive = currentDrive;

    /* change drive */
    CString pathWithoutDriveLetter = ChangeDriveAuto(path);

    /* read */
    result = fs->ReadDirectory(pathWithoutDriveLetter, prompt);

    /* restore current drive */
    currentDrive = saveDrive;
    return result;
}
