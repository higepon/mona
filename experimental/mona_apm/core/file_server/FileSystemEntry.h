/*!
    \file  FileSystemEntry.h
    \brief FileSystemEntry

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2005/01/23 update:$Date$
*/

#ifndef _FILE_SYSTEM_ENTRY_
#define _FILE_SYSTEM_ENTRY_

#ifdef MONA
#include <monapi.h>
#include <monapi/CString.h>
#else
#include "types.h"
#include "CString.h"
#endif


#include "FileDate.h"

/*----------------------------------------------------------------------
    FileSystemEntry
----------------------------------------------------------------------*/
class FileSystemEntry
{
public:
    FileSystemEntry() {}
    virtual ~FileSystemEntry() {}

public:
    virtual bool IsDirectory()          = 0;
    virtual dword GetSize()             = 0;
    virtual MonAPI::CString GetName()   = 0;
    virtual FileDate* GetCreateDate()   = 0;
    virtual FileDate* GetModifiedDate() = 0;
};

#endif
