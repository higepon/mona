/*!
    \file  FatFileSystemManager.h
    \brief FatFileSystemManager

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2005/03/17 update:$Date$
*/

#ifndef _FAT_FILE_SYSTEM_MANAGER_
#define _FAT_FILE_SYSTEM_MANAGER_

#include <monapi/io.h>
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

    void SetCurrentDirectory(const MonAPI::CString& directory) {this->currentDirectory = directory;}
    MonAPI::CString GetCurrentDirectory() const {return this->currentDirectory;}

protected:
    FDCDriver* fd;
    FSOperation* fs;
    MonAPI::CString currentDirectory;
};

#endif
