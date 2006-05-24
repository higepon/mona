/*!
    \file  FileSystemManager.h
    \brief FileSystemManager

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2005/03/17 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#ifndef _FILE_SYSTEM_MANAGER_
#define _FILE_SYSTEM_MANAGER_

#include <monapi/CString.h>
#ifdef ON_LINUX
#else
#include <monapi/messages.h>
#endif
#include "FileServer.h"
#include "File.h"

/*----------------------------------------------------------------------
    FileSystemManager
----------------------------------------------------------------------*/
class FileSystemManager
{
public:
    FileSystemManager() {}
    virtual  ~FileSystemManager() {};

    virtual  bool Initialize() = 0;
    virtual  bool ChangeDirectory(const MonAPI::CString& directory) = 0;
    virtual  void DeviceOn()  = 0;
    virtual  void DeviceOff() = 0;

    virtual  monapi_cmemoryinfo* ReadFile(const char* path, bool prompt)      = 0;
    virtual  monapi_cmemoryinfo* ReadDirectory(const char* path, bool prompt) = 0;

    virtual File* Open(const MonAPI::CString& file) = 0;
    virtual bool Close(File * file) = 0;

    virtual  void SetCurrentDirectory(const MonAPI::CString& directory) = 0;
    virtual  MonAPI::CString GetCurrentDirectory() const                = 0;
};

#endif
