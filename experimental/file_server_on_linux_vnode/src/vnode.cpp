#include "vnode.h"

using namespace std;

int VnodeManager::lookup(vnode* diretory, const string& file, vnode** found)
{
    return diretory->fs->lookup(diretory, file, found);
}

int VnodeManager::open(const string& name, int mode, bool create, vnode** entry)
{
    if (create)
    {
        // do something
    }

    return 1;
}


vnode* VnodeManager::alloc()
{
    vnode* v = new vnode;
    assert(v != NULL);
    return v;
}
