/*!
    \file  FileSystemManager.h
    \brief FileSystemManager

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2005/03/17 update:$Date$
*/

#ifndef _FILE_SYSTEM_MANAGER_
#define _FILE_SYSTEM_MANAGER_

#include <monapi/CString.h>
#include <monapi/messages.h>
#include "FileServer.h"

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

    virtual dword Open(const MonAPI::CString& file) = 0;
    virtual bool Close(const MonAPI::CString& file) = 0;

    virtual  void SetCurrentDirectory(const MonAPI::CString& directory) = 0;
    virtual  MonAPI::CString GetCurrentDirectory() const                = 0;
};

#endif
