/*!
    \file  FileSystemEntry.h
    \brief FileSystemEntry

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2005/01/23 update:$Date$
*/

#ifndef _DIRECTORY_
#define _DIRECTORY_

#include "FileSystemEntry.h"

/*----------------------------------------------------------------------
    Directory
----------------------------------------------------------------------*/
class Directory : public FileSystemEntry
{
public:
    Directory() {}
    virtual ~Directory() {}

public:
    virtual bool IsDirectory()          = 0;
    virtual dword GetSize()             = 0;
    virtual MonAPI::CString GetName()   = 0;
    virtual FileDate* GetCreateDate()   = 0;
    virtual FileDate* GetModifiedDate() = 0;
};

#endif
