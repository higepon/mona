#include "ProcessFileSystem.h"
#include "ProcessUtil.h"

using namespace std;
using namespace mona_helper;

ProcessFileSystem::ProcessFileSystem(VnodeManager* vmanager) : vmanager_(vmanager)
{
}

ProcessFileSystem::~ProcessFileSystem()
{
    for (Pnodes::iterator it = pnodes_.begin(); it != pnodes_.end(); it++)
    {
        Pnode* p = *it;
        delete p;
    }
    delete rootPnode_;
}

/*----------------------------------------------------------------------
    public interfaces
----------------------------------------------------------------------*/
int ProcessFileSystem::initialize()
{
    rootPnode_ = new Pnode(Pnode::ROOT_DIRECTORY, string(""));

    root_ = vmanager_->alloc();
    root_->fnode  = rootPnode_;
    root_->fs     = this;
    root_->type = Vnode::DIRECTORY;
    pnodes_.push_back(new Pnode(1, string("init")));
    pnodes_.push_back(new Pnode(2, string("sh")));
    pnodes_.push_back(new Pnode(3, string("httpd")));
    return MONA_SUCCESS;
}

int ProcessFileSystem::lookup(Vnode* diretory, const string& file, Vnode** found, int type)
{
    if (diretory->type != Vnode::DIRECTORY) return MONA_ERROR_INVALID_ARGUMENTS;
    if (type != Vnode::REGULAR) return MONA_ERROR_ENTRY_NOT_FOUND;

    Pnode* pnode = (Pnode*)diretory->fnode;
    if (pnode->tid != Pnode::ROOT_DIRECTORY) return MONA_ERROR_ENTRY_NOT_FOUND;

    Vnode* v = vmanager_->cacher()->lookup(diretory, file);
    if (v != NULL && v->type == type)
    {
        *found = v;
        return MONA_SUCCESS;
    }

    for (Pnodes::iterator it = pnodes_.begin(); it != pnodes_.end(); it++)
    {
        if ((*it)->name == file)
        {
            Vnode* v = vmanager_->alloc();
            v->fnode = (*it);
            v->type = Vnode::REGULAR;
            v->fs = this;
            vmanager_->cacher()->add(diretory, file, v);
            *found = v;
            return MONA_SUCCESS;
        }
    }
    return MONA_ERROR_ENTRY_NOT_FOUND;
}

int ProcessFileSystem::open(Vnode* file, int mode)
{
    return MONA_SUCCESS;
}

int ProcessFileSystem::create(Vnode* dir, const string& file)
{
    return MONA_FAILURE;
}

int ProcessFileSystem::read(Vnode* file, struct io::Context* context)
{
    Pnode* p = (Pnode*)file->fnode;
    monapi_cmemoryinfo* memory = context->memory;
    memcpy(memory->Data, &(p->tid), sizeof(dword));
    context->resultSize = sizeof(dword);
    return MONA_SUCCESS;
}

int ProcessFileSystem::seek(Vnode* file, dword offset, dword origin)
{
    return MONA_SUCCESS;
}

int ProcessFileSystem::close(Vnode* file)
{
    return MONA_SUCCESS;
}

int ProcessFileSystem::stat(Vnode* file, Stat* st)
{
    st->size = 0;
    return MONA_SUCCESS;
}

Vnode* ProcessFileSystem::getRoot() const
{
    return root_;
}

int ProcessFileSystem::readdir(Vnode* dir, monapi_cmemoryinfo** entries)
{
    Pnode* directory = (Pnode*)dir->fnode;
    if (directory->tid != Pnode::ROOT_DIRECTORY) return MONA_ERROR_ENTRY_NOT_FOUND;

    int size = pnodes_.size();
    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    if (!monapi_cmemoryinfo_create(ret, sizeof(int) + size * sizeof(monapi_directoryinfo), MONAPI_FALSE))
    {
        monapi_cmemoryinfo_delete(ret);
        return MONA_ERROR_MEMORY_NOT_ENOUGH;
    }
    memcpy(ret->Data, &size, sizeof(int));
    monapi_directoryinfo* p = (monapi_directoryinfo*)&ret->Data[sizeof(int)];

    for (Pnodes::iterator it = pnodes_.begin(); it != pnodes_.end(); it++)
    {
        monapi_directoryinfo di;

        di.size = 0;
        strcpy(di.name, (*it)->name.c_str());
        di.attr = 0;
        *p = di;
        p++;
    }

    *entries = ret;
    return MONA_SUCCESS;
}

void ProcessFileSystem::destroyVnode(Vnode* vnode)
{
//    don't delete entry because pnodes will be deleted in destructor
//    Pnode* entry = (Pnode*)vnode->fnode;
//    delete entry;
    delete vnode;
}
