/*!
    \file  File.h
    \brief File

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2004/11/06 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#ifndef _ISO9660_FILE_
#define _ISO9660_FILE_

#include "File.h"
#include "ISO9660.h"
#include "ISO9660Directory.h"
#include "ISO9660FileSystem.h"

class ISO9660FileSystem;

/*----------------------------------------------------------------------
    ISO9660File
----------------------------------------------------------------------*/
class ISO9660File : public File
{
public:
    ISO9660File(ISO9660FileSystem* fs);
    virtual ~ISO9660File();

public:
    virtual bool IsDirectory() {return false;}
    virtual dword GetSize() {return this->attribute.size;}
    virtual MonAPI::CString GetName() {return this->name;}
    virtual FileDate* GetCreateDate() {return &this->createDate;}
    virtual FileDate* GetModifiedDate() {return &this->modifiedDate;}
    virtual dword Read(void* buffer, dword size);
    virtual dword Write(void* buffer, dword size);
    virtual bool Seek(dword offset, dword origin);

public:
    virtual void SetName(MonAPI::CString name) {this->name = name;}

public:
    ISO9660Directory* parent;
    ISO9660Attribute attribute;
    bool hasDetail;

private:
    MonAPI::CString name;
    FileDate createDate;
    FileDate modifiedDate;
    dword pointer;
    ISO9660FileSystem* fs;
};

#endif
