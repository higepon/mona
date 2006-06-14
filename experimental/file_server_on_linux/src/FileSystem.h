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
#include "types.h"

class Vnode;       // defined somewhere
namespace io
{
    struct Context; // defined somewhere
};

/*----------------------------------------------------------------------
    FileSystem
----------------------------------------------------------------------*/
class FileSystem
{
public:
    FileSystem() {}
    virtual ~FileSystem() {}

public:
    virtual int initialize()                                                               = 0;
    virtual int lookup(Vnode* diretory, const std::string& file, Vnode** found, int type) = 0;
    virtual int open(Vnode* file, int mode)                                               = 0;
    virtual int read(Vnode* file, struct io::Context* context)                            = 0;
    //    virtual int readdir(Vnode* directory, std::vector<FileSystemEntr*>* entries)          = 0;
    virtual int seek(Vnode* file, dword offset, dword origin)                             = 0;
    virtual int close(Vnode* file)                                                        = 0;
};

#endif
