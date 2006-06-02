#include "vnode.h"

using namespace std;

int VnodeManager::lookup(Vnode* diretory, const string& file, Vnode** found)
{
    return diretory->fs->lookup(diretory, file, found);
}

int VnodeManager::open(const string& name, int mode, bool create, Vnode** entry)
{
    if (create)
    {
        // do something
    }

    return 1;
}


Vnode* VnodeManager::alloc()
{
    Vnode* v = new Vnode;
    assert(v != NULL);
    return v;
}
