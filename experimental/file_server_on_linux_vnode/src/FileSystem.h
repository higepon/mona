/*!
    \file  FileSystem.h
    \brief FileSystem

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2005/01/23 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#ifndef _FILE_SYSTEM_
#define _FILE_SYSTEM_

#include <string>
#include "error.h"
#include "File.h"

class Vnode; // defined somewhere

/*----------------------------------------------------------------------
    FileSystem
----------------------------------------------------------------------*/
class FileSystem
{
public:
    FileSystem() {}
    virtual ~FileSystem() {}

public:
    virtual File* Open(const MonAPI::CString& path, int mode)                       = 0;
    virtual bool Close(File* file)                                                  = 0;
    virtual bool CreateFile(const MonAPI::CString& path)                            = 0;
    virtual bool RemoveFile(const MonAPI::CString& path)                            = 0;
    virtual bool CreateDirectory(const MonAPI::CString& path)                       = 0;
    virtual bool RemoveDirectory(const MonAPI::CString& path)                       = 0;
    virtual bool IsExistDirectory(const MonAPI::CString& path)                      = 0;
    virtual _A<FileSystemEntry*> GetFileSystemEntries(const MonAPI::CString& path)  = 0;
    virtual int GetLastError()                                                      = 0;
    virtual int lookup(Vnode* diretory, const std::string& file, Vnode** found)     = 0;
    virtual int open(Vnode* file, int mode)                                         = 0;
};

#endif
