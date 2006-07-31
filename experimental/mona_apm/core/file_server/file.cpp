#include "ISO9660FileSystemManager.h"
#include "FatFileSystemManager.h"
#include <sys/BinaryTree.h>

using namespace MonAPI;

static FileSystemManager* isofs;
static FileSystemManager* fatfs;
static FileSystemManager* fs;
int currentDrive;

BinaryTree<File*> fileTree;

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

CString GetCurrentDirectory()
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

void Initialize()
{
    /* current drive */
    currentDrive = DRIVE_FD0;
    isofs = new ISO9660FileSystemManager();

    fatfs = new FatFileSystemManager();
    isofs->SetCurrentDirectory("/");
    fatfs->SetCurrentDirectory("/");

    /* まずはCDブートを試みる */
    if (ChangeDrive(DRIVE_CD0) == MONA_FAILURE)
    {
        ChangeDrive(DRIVE_FD0);
        fatfs->Initialize();
    } else {
        isofs->Initialize();
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

dword Open(const CString& path)
{
    static dword id = 0;

    /* save current drive */
    int saveDrive = currentDrive;

    /* change drive */
    CString pathWithoutDriveLetter = ChangeDriveAuto(path);

    /* read */
    File* file = fs->Open(pathWithoutDriveLetter.toUpper());

    /* file not found */
    if (file == NULL) return 0;

    id++;
    fileTree.add(id, file);


    /* restore current drive */
    currentDrive = saveDrive;
    return id;
}

bool Seek(dword id, dword position, dword flag)
{
    File* file = fileTree.get(id);

    if (file == NULL) return false;

    return file->Seek(position, flag);
}

monapi_cmemoryinfo* Read(dword id, dword size)
{
    File* file = fileTree.get(id);

    if (file == NULL) return 0;

    byte *buf = (byte*)malloc(size * sizeof(byte));
    if (buf == NULL) return 0;
    dword readSize = file->Read(buf, size);
//printf("File Server: %d bytes read.\n", readSize);

    if (readSize == 0)
    {
	free(buf);
	return 0;
    }

    dword copySize;
    if (readSize < size)
	copySize = readSize;
    else
	copySize = size;
//printf("File Server: copySize = %d\n", copySize);

    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    if (!monapi_cmemoryinfo_create(ret, copySize, false))
    {
	free(buf);
        monapi_cmemoryinfo_delete(ret);
        return 0;
    }

    for (dword i = 0; i < copySize; i++)
    {
	ret->Data[i] = buf[i];
    }
    free(buf);

#if 0
// 大切な実行時間の無駄
    for (dword i = 0; i < ret->Size; i++)
    {
        printf("[%x]", ret->Data[i]);
    }
#endif

    if (copySize == 0)
    {
        monapi_cmemoryinfo_delete(ret);
        return 0;
    }

    return ret;
}

dword GetFileSize(dword id)
{
    File* file = fileTree.get(id);

    if (file == NULL) return false;

    return file->GetSize();
}

bool Close(dword id)
{
    File* file = fileTree.get(id);

    if (file == NULL) return false;

    fileTree.remove(id);

    return fs->Close(file);
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
    result = fs->ReadFile(pathWithoutDriveLetter.toUpper(), prompt);

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
    result = fs->ReadDirectory(pathWithoutDriveLetter.toUpper(), prompt);

    /* restore current drive */
    currentDrive = saveDrive;
    return result;
}
