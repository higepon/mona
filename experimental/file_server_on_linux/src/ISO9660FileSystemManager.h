/*!
    \file  ISO9660FileSystemManager.h
    \brief ISO9660FileSystemManager

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2005/03/17 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#ifndef _ISO9660_FILE_SYSTEM_MANAGER_
#define _ISO9960_FILE_SYSTEM_MANAGER_

#ifndef ON_LINUX
#include <monapi/io.h>
#endif

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

    virtual File* Open(const MonAPI::CString& file);
    virtual bool Close(File* file);

    void SetCurrentDirectory(const MonAPI::CString& directory) {this->currentDirectory = directory;}
    MonAPI::CString GetCurrentDirectory() const {return this->currentDirectory;}

protected:
    IDEDriver* cd;
    ISO9660FileSystem* fs;
    MonAPI::CString currentDirectory;
    bool initialized;
};

#endif
