/*!
    \file  FatFileSystem.cpp
    \brief FatFileSystemManager

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2005/03/17 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#include "FatFileSystemManager.h"

#ifdef ON_LINUX
#include "HList.h"
#include <monapi/messages.h>
#endif

using namespace MonAPI;

/*----------------------------------------------------------------------
    FatFileSystemManager
----------------------------------------------------------------------*/
FatFileSystemManager::FatFileSystemManager()
{
}

FatFileSystemManager::~FatFileSystemManager()
{
}

bool FatFileSystemManager::Initialize()
{
#ifndef ON_LINUX
    syscall_get_io();

    monapi_set_irq(6, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(6);
#endif

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

File* FatFileSystemManager::Open(const MonAPI::CString& file)
{
    printf("FatFileSystemManager::Open not implemented\n");
    return NULL;
}

bool FatFileSystemManager::Close(File* file)
{
    printf("FatFileSystemManager::Close not implemented\n");
    return false;
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
