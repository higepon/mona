/*!
    \file  File.h
    \brief File

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2004/11/06 update:$Date$
*/

#ifndef _MONA_FILE_
#define _MONA_FILE_

#include <sys/types.h>
#include <monapi/CString.h>

namespace FileServer {

class FSEntry
{
public:
    FSEntry() {}
    virtual ~FSEntry() {}

public:
    virtual bool IsDirectory() = 0;

    virtual dword GetSize() const
    {
        return this->size;
    }

    virtual MonAPI::CString GetName()
    {
        return this->name;
    }

private:
    dword size;
    MonAPI::CString name;
};

class Directory : public FSEntry
{
public:
    Directory();
    virtual ~Directory();

};

class File : public FSEntry
{
public:
    File();
    virtual ~File();
};

class ISO9660File : public File
{
public:
    ISO9660File();
    virtual ~ISO9660File();

};

enum
{
    FS_READ_ONLY_ERROR = 1
};

class FileSystem
{
public:
    virtual File* Open(const char* path, int mode)      = 0;
    virtual bool CreateDirectory(const char* directory) = 0;
    virtual bool RemoveDirectory(const char* directory) = 0;
    virtual bool CreateFile(const char* path)           = 0;
    virtual bool RemoveFile(const char* file)           = 0;
    virtual int GetLastError() const {return lastError;}

protected:
    int lastError;
};

class ReadOnlyFileSystem : public FileSystem
{
public:
    virtual bool CreateDirectory(const char* directory) {this->lastError = FS_READ_ONLY_ERROR; return false;}
    virtual bool RemoveDirectory(const char* directory) {this->lastError = FS_READ_ONLY_ERROR; return false;}
    virtual bool CreateFile(const char* path)           {this->lastError = FS_READ_ONLY_ERROR; return false;}
    virtual bool RemoveFile(const char* file)           {this->lastError = FS_READ_ONLY_ERROR; return false;}
};

}


namespace ISO {

/*----------------------------------------------------------------------
    File
----------------------------------------------------------------------*/
class File
{
public:
    virtual dword Read(void* buffer, dword size) = 0;
    virtual bool Seek(int position, int flag)    = 0;
    virtual dword GetSize() const                = 0;
//    virtual const char* GetName()                = 0;

public:
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
};

}

#endif
