#include "vnode.h"

using namespace std;

int Vnode::lookup(vnode* diretory, const string& file, vnode** found)
{
    return diretory->fs->lookup(diretory, file, found);
}

vnode* Vnode::alloc()
{
    vnode* v = new vnode;
    assert(v != NULL);
    return v;
}
