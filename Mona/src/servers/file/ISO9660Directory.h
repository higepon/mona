/*!
    \file  FileSystemEntry.h
    \brief FileSystemEntry

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2005/01/23 update:$Date$
*/

#ifndef _ISO9660_DIRECTORY_
#define _ISO9660_DIRECTORY_

#include "ISO9660.h"
#include "Directory.h"

/*----------------------------------------------------------------------
    ISO9660Directory
----------------------------------------------------------------------*/
class ISO9660Directory : public Directory
{
public:
    ISO9660Directory() : hasDetail(false) {}
    virtual ~ISO9660Directory() {}

public:
    virtual bool IsDirectory() {return true;}
    virtual dword GetSize() {return 0;}
    virtual MonAPI::CString GetName()   {return this->name;}
    virtual FileDate* GetCreateDate()   {return &createDate;}
    virtual FileDate* GetModifiedDate() {return &modifiedDate;}

public:
    virtual void SetName(MonAPI::CString name) {this->name = name;}


public:
    ISO9660Attribute attribute;
    ISO9660Directory* parent;
    HList<FileSystemEntry*> children;
    bool hasDetail;

private:
    MonAPI::CString name;
    FileDate createDate;
    FileDate modifiedDate;

};

#endif
