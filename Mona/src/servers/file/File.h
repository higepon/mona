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

#include "FileSystemEntry.h"

/*----------------------------------------------------------------------
    File
----------------------------------------------------------------------*/
class File : public FileSystemEntry
{
public:
    File() {}
    virtual ~File() {}

public:
    virtual bool IsDirectory()                    = 0;
    virtual dword GetSize()                       = 0;
    virtual MonAPI::CString GetName()             = 0;
    virtual FileDate* GetCreateDate()             = 0;
    virtual FileDate* GetModifiedDate()           = 0;
    virtual dword Read(void* buffer, dword size)  = 0;
    virtual dword Write(void* buffer, dword size) = 0;
    virtual bool Seek(dword offset, dword origin) = 0;
};

#endif
