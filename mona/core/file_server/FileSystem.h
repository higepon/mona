/*!
    \file  FileSystem.h
    \brief FileSystem

    Copyright (c) 2005 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision: 3123 $
    \date   create:2005/01/23 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#ifndef _FILE_SYSTEM_
#define _FILE_SYSTEM_

#include <string>
#include "error.h"
#include "sys/types.h"
#include "monapi/messages.h"

class Vnode;       // defined somewhere
namespace io
{
    class Context; // defined somewhere
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
    virtual int initialize()                                                              = 0;
    virtual int lookup(Vnode* diretory, const std::string& file, Vnode** found, int type) = 0;
    virtual int open(Vnode* file, int mode)                                               = 0;
    virtual int read(Vnode* file, struct io::Context* context)                            = 0;
    virtual int write(Vnode* file, struct io::Context* context)                           = 0;
    virtual int create(Vnode* dir, const std::string& file)                               = 0;
    virtual int readdir(Vnode* directory, monapi_cmemoryinfo** entries)                   = 0;
    virtual int seek(Vnode* file, uint32_t offset, uint32_t origin)                       = 0;
    virtual int close(Vnode* file)                                                        = 0;
    virtual int delete_file(Vnode* file)                                                  = 0;
    virtual int stat(Vnode* file, Stat* st)                                               = 0;
    virtual Vnode* getRoot() const                                                        = 0;
    virtual void destroyVnode(Vnode* vnode)                                               = 0;
};

#endif
