#include "vnode.h"

int Vnode::lookup(vnode* diretory, const MonAPI::CString& file, vnode** found)
{
    return diretory->fs->lookup(diretory, file, found);
}
