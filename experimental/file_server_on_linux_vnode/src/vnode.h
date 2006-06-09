/*!
    \file  vnode.h
    \brief vnode

    Copyright (c) 2006 Higepon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2006/05/24 update:$Date$
*/

#ifndef _VNODE_
#define _VNODE_

#include <map>
#include "monapi/CString.h"
#include "monapi/messages.h"
#include "FileSystem.h"
#include "types.h"


// enum vtype
// {
//     VNON,
//     VREG,
//     VDIR,
//     VBLK,
//     VCHR,
//     VLNK,
//     VSOCK,
//     VFIFO,
//     VBAD
// };

// typedef struct vnode
// {
//     enum vtype v_type;
//     FileSystem* fs;
//     void* fnode;
// };

namespace io
{

typedef struct Context
{
    Context() : memory(NULL), offset(0), size(0) {}
    dword tid;
    monapi_cmemoryinfo* memory;
    dword offset;
    dword origin;
    dword size;
    dword resultSize;
} Context;

typedef struct
{
    Vnode* vnode;
    io::Context context;
} FileInfo;

};
class Vnode
{
public:
    int type;
    FileSystem* fs;
    void* fnode;
    enum
    {
        NONE,
        REGULAR,
        DIRECTORY,
        LINK,
        BAD
    };
};


class VnodeManager
{
public:
    VnodeManager();
    ~VnodeManager();

public:
    void setRoot(Vnode* root) {root_ = root;}
    int lookup(Vnode* diretory, const std::string& file, Vnode** found);
    int open(const std::string& name, int mode, bool create, dword tid, dword* fileID);
    int read(dword fileID, dword size, monapi_cmemoryinfo** mem);
    int seek(dword fileID, dword offset, dword origin);
    dword fileID(Vnode* file , dword tid) {return (dword)file | tid;} // temporary
    Vnode* alloc();

private:
    typedef std::map<dword, io::FileInfo*> FileInfoMap;
    Vnode* root_;
    FileInfoMap fileInfoMap_;
};


#endif // _VNODE_
