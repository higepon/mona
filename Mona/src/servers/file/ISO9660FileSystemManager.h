/*!
    \file  ISO9660FileSystemManager.h
    \brief ISO9660FileSystemManager

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2005/03/17 update:$Date$
*/

#ifndef _ISO9660_FILE_SYSTEM_MANAGER_
#define _ISO9960_FILE_SYSTEM_MANAGER_

#include <monapi/io.h>
#include "FileSystemManager.h"
#include "IDEDriver.h"
#include "ISO9660FileSystem.h"

/*----------------------------------------------------------------------
    ISO9660FileSystemManager
----------------------------------------------------------------------*/
class ISO9660FileSystemManager : public FileSystemManager
{
public:
    ISO9660FileSystemManager();
    virtual ~ISO9660FileSystemManager();

    bool Initialize();
    bool ChangeDirectory(const MonAPI::CString& directory);
    void DeviceOn();
    void DeviceOff();

    monapi_cmemoryinfo* ReadFile(const char* path, bool prompt);
    monapi_cmemoryinfo* ReadDirectory(const char* path, bool prompt);

    void SetCurrentDirectory(const MonAPI::CString& directory) {this->currentDirectory = directory;}
    MonAPI::CString GetCurrentDirectory() const {return this->currentDirectory;}

protected:
    IDEDriver* cd;
    ISO9660FileSystem* fs;
    MonAPI::CString currentDirectory;
    bool initialized;
};

#endif
