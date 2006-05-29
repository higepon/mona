#include "vnode.h"

using namespace std;

int Vnode::lookup(vnode* diretory, const string& file, vnode** found)
{
    return diretory->fs->lookup(diretory, file, found);
}

int Vnode::open(const string& name, int mode, bool create, vnode** entry)
{
    if (create)
    {
        // do something
    }

    return 1;
}


vnode* Vnode::alloc()
{
    vnode* v = new vnode;
    assert(v != NULL);
    return v;
}
