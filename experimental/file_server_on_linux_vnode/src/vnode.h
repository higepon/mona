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


enum vtype
{
    VNON,
    VREG,
    VDIR,
    VBLK,
    VCHR,
    VLNK,
    VSOCK,
    VFIFO,
    VBAD
};

typedef struct vnode
{
    enum vtype v_type;
    FileSystem* fs;
};

class Vnode
{
private:
    Vnode() {}
    ~Vnode() {}

public:
    static int lookup(vnode* diretory, const MonAPI::CString& file, vnode** found);
};


#endif // _VNODE_
