/*!
    \file  ISO9660FileSystem.cpp
    \brief ISO9660FileSystemManager

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2005/03/17 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#include <monapi/messages.h>
#include "ISO9660FileSystemManager.h"

#ifdef ON_LINUX
#include <stdio.h>
#include <string.h>
#endif

using namespace MonAPI;
#define IRQ_PRIMARY   14
#define IRQ_SECONDARY 15

/*----------------------------------------------------------------------
    ISO9660FileSystemManager
----------------------------------------------------------------------*/
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

#ifdef ON_LINUX
    /* IDE Driver */
    this->cd = new IDEDriver(IRQ_PRIMARY, IRQ_SECONDARY);
    this->cd->open();
#else
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

#endif
    this->fs = new ISO9660FileSystem(cd, new VnodeCacher());
    /* initialize ISO9660 FS */
    if (!this->fs->Initialize())
    {
        printf("CD Boot Initialize Error = %d\n", this->fs->GetLastError());
        delete this->fs;
        delete this->cd;
        return false;
    }
    this->initialized = true;
    return true;
}

File* ISO9660FileSystemManager::Open(const MonAPI::CString& file)
{
    return fs->Open(file, 0);
}

bool ISO9660FileSystemManager::Close(File *file)
{
    return fs->Close(file);
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
    printf("%s %s:%d\n", __func__, path, __LINE__);fflush(stdout);
    if (!this->Initialize()) return NULL;

    /* file open */
    File* file = fs->Open(path, 0);

    if (file == NULL)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        if (prompt) printf("read:file not found\n");
        return NULL;
    }

    /* allocate memory */
    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    if (!monapi_cmemoryinfo_create(ret, file->GetSize() + 1, prompt))
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
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
        printf("%s\n", (const char*)file->GetName());
        di.attr = file->IsDirectory() ? ATTRIBUTE_DIRECTORY : 0;
        *p = di;
        p++;
    }
    END_FOREACH
    return ret;
}
