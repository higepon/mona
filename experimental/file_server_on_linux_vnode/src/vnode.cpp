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

    // process current path

    // now fullpath only
    if (name.compare(0, 0, "/") != 0) return MONA_ERROR_INVALID_ARGUMENTS;

    Vnode* found;
    if (lookup(root_, name, &found) != MONA_OK)
    {
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }

    return found->fs->open(found, mode);
}


Vnode* VnodeManager::alloc()
{
    Vnode* v = new Vnode;
    assert(v != NULL);
    return v;
}
