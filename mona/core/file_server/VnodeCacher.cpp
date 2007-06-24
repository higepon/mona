#include "VnodeCacher.h"

using namespace std;

typedef pair<Vnode*, EntriesMap*> vpair;
typedef pair<string, Vnode*> spair;
typedef vector<Vnode*> Vnodes;

VnodeCacher::VnodeCacher()
{
    directories_ = new DirectoriesMap();

}

VnodeCacher::~VnodeCacher()
{
    Vnodes vnodes;
    // enumrate all Vnodes
    for (DirectoriesMap::const_iterator it = directories_->begin(); it != directories_->end(); ++it)
    {
        vnodes.push_back((*it).first);
        EntriesMap* entries = (*it).second;
        for (EntriesMap::iterator i = entries->begin(); i != entries->end(); i++)
        {
            vnodes.push_back((*i).second);
        }
        delete entries;
    }

    // enumrate unique Vnodes
    sort(vnodes.begin(), vnodes.end());
    Vnodes::iterator v = unique(vnodes.begin(), vnodes.end());
    vnodes.erase(v ,vnodes.end());

    for (Vnodes::const_iterator it = vnodes.begin(); it != vnodes.end(); ++it)
    {
        Vnode* v = (*it);
        v->fs->destroyVnode(v);
    }
    delete directories_;
}

Vnode* VnodeCacher::lookup(Vnode* directory, const string& name)
{
    if (directory->type != Vnode::DIRECTORY) return NULL;

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

void VnodeCacher::add(Vnode* directory, const string& name, Vnode* entry)
{
    if (directory->type != Vnode::DIRECTORY) return;

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
        _printf("%s:%d already exists vnode\n", __FILE__, __LINE__);
        exit(-1);
    }

    entries->insert(spair(name, entry));
    return;
}

void VnodeCacher::remove(Vnode* directory, const string& name)
{
    if (directory->type != Vnode::DIRECTORY) return;

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
