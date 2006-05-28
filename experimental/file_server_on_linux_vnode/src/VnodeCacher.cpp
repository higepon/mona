#include "VnodeCacher.h"

using namespace std;

typedef pair<vnode*, EntriesMap*> vpair;
typedef pair<string, vnode*> spair;

VnodeCacher::VnodeCacher()
{
    directories_ = new DirectoriesMap();

}

VnodeCacher::~VnodeCacher()
{
    delete directories_;
}

vnode* VnodeCacher::lookup(vnode* directory, const string& name)
{
    if (directory->v_type != VDIR) return NULL;

    DirectoriesMap::iterator it = directories_->find(directory);
    EntriesMap* entries;
    if (it == directories_->end())
    {
        return NULL;
    }
    else
    {
        entries = (*it).second;
    }

    EntriesMap::iterator eit = entries->find(name);
    return eit == entries->end() ? NULL : (*eit).second;
}

void VnodeCacher::add(vnode* directory, const string& name, vnode* entry)
{
    if (directory->v_type != VDIR) return;

    DirectoriesMap::iterator it = directories_->find(directory);
    EntriesMap* entries;
    if (it == directories_->end())
    {
        entries = new EntriesMap;
        directories_->insert(vpair(directory, entries));
    }
    else
    {
        entries = (*it).second;
    }

    EntriesMap::iterator eit = entries->find(name);
    if (eit != entries->end())
    {
        printf("%s:%d already exists vnode\n", __FILE__, __LINE__);
        exit(-1);
    }

    entries->insert(spair(name, entry));
    return;
}

void VnodeCacher::remove(vnode* directory, const string& name)
{
    if (directory->v_type != VDIR) return;

    DirectoriesMap::iterator it = directories_->find(directory);
    EntriesMap* entries;
    if (it == directories_->end())
    {
        return;
    }
    else
    {
        entries = (*it).second;
    }

    entries->erase(name);
    return;
}
