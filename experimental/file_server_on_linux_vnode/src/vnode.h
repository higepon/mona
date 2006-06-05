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

#include "monapi/CString.h"
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

typedef struct
{
    void* pointer;
    dword size;
} Buffer;

typedef struct Context
{
    dword tid;
    Buffer* buffer;
    dword offset;
    dword size;
} Context;

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
    int open(const std::string& name, int mode, bool create, Vnode** entry);
    int read(Vnode* file, io::Context* context);
    Vnode* alloc();

private:
    Vnode* root_;
};


#endif // _VNODE_
