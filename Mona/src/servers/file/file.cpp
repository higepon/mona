#include "ISO9660FileSystemManager.h"
#include "FatFileSystemManager.h"

using namespace MonAPI;

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
        ChangeDrive(DRIVE_FD0);
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
