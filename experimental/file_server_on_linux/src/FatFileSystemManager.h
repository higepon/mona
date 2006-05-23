/*!
    \file  FatFileSystemManager.h
    \brief FatFileSystemManager

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2005/03/17 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#ifndef _FAT_FILE_SYSTEM_MANAGER_
#define _FAT_FILE_SYSTEM_MANAGER_

#ifdef ON_LINUX
#else
#include <monapi/io.h>
#endif
#include "FileSystemManager.h"
#include "FDCDriver.h"
#include "FSOperation.h"

/*----------------------------------------------------------------------
    FatFileSystemManager
----------------------------------------------------------------------*/
class FatFileSystemManager : public FileSystemManager
{
public:
    FatFileSystemManager();
    virtual ~FatFileSystemManager();

    bool Initialize();
    bool ChangeDirectory(const MonAPI::CString& directory);
    void DeviceOn();
    void DeviceOff();

    monapi_cmemoryinfo* ReadFile(const char* path, bool prompt);
    monapi_cmemoryinfo* ReadDirectory(const char* path, bool prompt);

    File* Open(const MonAPI::CString& file);
    bool Close(File* file);

    void SetCurrentDirectory(const MonAPI::CString& directory) {this->currentDirectory = directory;}
    MonAPI::CString GetCurrentDirectory() const {return this->currentDirectory;}

protected:
    FDCDriver* fd;
    FSOperation* fs;
    bool initialized;
    MonAPI::CString currentDirectory;
};

#endif
