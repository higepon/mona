#include "FAT12FileSystem.h"

using namespace std;
using namespace FatFS;

FAT12FileSystem::FAT12FileSystem(FDCDriver* drive, VnodeManager* vmanager) : drive_(drive), vmanager_(vmanager), fat_(NULL)
{
    fd_ = drive;
}

FAT12FileSystem::~FAT12FileSystem()
{
    if (fat_ != NULL) delete fat_;
//    delete root_;
}

/*----------------------------------------------------------------------
    public interfaces
----------------------------------------------------------------------*/
int FAT12FileSystem::initialize()
{
#ifdef ON_LINUX

#else
    syscall_get_io();

    monapi_set_irq(6, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(6, 0);
    if (!fd_->isInserted(0))
    {
        return MONA_ERROR_ON_DEVICE;
    }
#endif

    fat_ = new FatStorage();

    if (!fat_->initialize(drive_))
    {
        delete fat_;
        fat_ = NULL;
        return MONA_ERROR_ON_DEVICE;
    }

    fatroot_ = fat_->getRootDirectory();
    current_ = fatroot_;

    if (current_ == NULL)
    {
        delete fat_;
        fat_ = NULL;
        return MONA_ERROR_ON_DEVICE;
    }
    root_ = vmanager_->alloc();
    root_->fnode  = fatroot_;
    root_->fs     = this;
    root_->type = Vnode::DIRECTORY;
    return MONA_SUCCESS;
}

int FAT12FileSystem::lookup(Vnode* diretory, const string& file, Vnode** found, int type)
{
    if (diretory->type != Vnode::DIRECTORY) return MONA_ERROR_INVALID_ARGUMENTS;
    Vnode* v = vmanager_->cacher()->lookup(diretory, file);
    if (v != NULL && v->type == type)
    {
        *found = v;
        return MONA_SUCCESS;
    }

    if (type != Vnode::REGULAR && type != Vnode::DIRECTORY) return MONA_ERROR_ENTRY_NOT_FOUND;

    int entry;
    int cursor;
    current_ = (Directory*)(diretory->fnode);
    string tmp = file;
    Directory* d = searchFile((char*)(tmp.c_str()), &entry, &cursor);

    if (d == NULL)
    {
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }

    if (type == Vnode::REGULAR)
    {
        File* f = d->getFile(entry);
        if (f == NULL)
        {
            freeDirectory(d);
            return MONA_ERROR_ENTRY_NOT_FOUND;
        }
        Vnode* newVnode = vmanager_->alloc();
        newVnode->fnode  = f;
        newVnode->type = type;
        newVnode->fs = this;
        vmanager_->cacher()->add(diretory, file, newVnode);
        *found = newVnode;
        return MONA_SUCCESS;
    }
    else // Vnode::DIRECTORY
    {
        Directory* dir = d->getDirectory(entry);
        if (dir == NULL)
        {
            freeDirectory(d);
            return MONA_ERROR_ENTRY_NOT_FOUND;
        }
        Vnode* newVnode = vmanager_->alloc();
        newVnode->fnode  = dir;
        newVnode->type = type;
        newVnode->fs = this;
        vmanager_->cacher()->add(diretory, file, newVnode);
        *found = newVnode;
        return MONA_SUCCESS;
    }
}

int FAT12FileSystem::open(Vnode* file, int mode)
{
    return MONA_SUCCESS;
}

int FAT12FileSystem::create(Vnode* dir, const string& file)
{
    Directory* p = (Directory*)dir->fnode;
    int entry = p->newFile((byte*)file.c_str(), 0);
    if (-1 == entry)
    {
        return MONA_FAILURE;
    }
    return MONA_SUCCESS;
}

int FAT12FileSystem::read(Vnode* file, struct io::Context* context)
{
    if (file->type != Vnode::REGULAR) return MONA_FAILURE;
    File* f = (File*)file->fnode;
    monapi_cmemoryinfo* memory = context->memory;
    dword offset = context->offset;
    dword readSize = context->size;
    dword rest = f->size() - offset;

    if (rest < readSize)
    {
        readSize = rest;
    }

    f->seek(offset, SEEK_SET);
    f->read(memory->Data, readSize);
    return MONA_SUCCESS;
}

int FAT12FileSystem::write(Vnode* file, struct io::Context* context)
{
    if (file->type != Vnode::REGULAR) return MONA_FAILURE;
    File* f = (File*)file->fnode;
    monapi_cmemoryinfo* memory = context->memory;
    dword offset = context->offset;
    dword writeSize = context->size;
    f->seek(offset, SEEK_SET);
    f->write(memory->Data, writeSize);
    f->flush();
    return MONA_SUCCESS;
}

int FAT12FileSystem::seek(Vnode* file, dword offset, dword origin)
{
    return MONA_SUCCESS;
}

int FAT12FileSystem::close(Vnode* file)
{
    return MONA_SUCCESS;
}

int FAT12FileSystem::stat(Vnode* file, Stat* st)
{
    File* f = (File*)file->fnode;
    st->size = f->size();
    return MONA_SUCCESS;
}

Vnode* FAT12FileSystem::getRoot() const
{
    return root_;
}

int FAT12FileSystem::readdir(Vnode* dir, monapi_cmemoryinfo** entries)
{
    deviceOn();
    Directory* target = (Directory*)dir->fnode;

    lsinfo_.entry = target->getHeadEntry();
    lsinfo_.p     = target;

    typedef vector<monapi_directoryinfo*> Files;
    Files files;
    monapi_directoryinfo di;
    while (readdirInternal(di.name, &di.size, &di.attr) == MONA_SUCCESS)
    {
        files.push_back(new monapi_directoryinfo(di));
    }
    deviceOff();

    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    int size = files.size();
    if (!monapi_cmemoryinfo_create(ret, sizeof(int) + size * sizeof(monapi_directoryinfo), MONAPI_FALSE))
    {
        monapi_cmemoryinfo_delete(ret);
        for (Files::iterator it = files.begin(); it != files.end(); it++)
        {
            delete (*it);
        }
        return MONA_ERROR_MEMORY_NOT_ENOUGH;
    }

    memcpy(ret->Data, &size, sizeof(int));
    monapi_directoryinfo* p = (monapi_directoryinfo*)&ret->Data[sizeof(int)];

    for (Files::iterator it = files.begin(); it != files.end(); it++)
    {
        memcpy(p, (*it), sizeof(monapi_directoryinfo));
        delete (*it);
        p++;
    }
    *entries = ret;
    return MONA_SUCCESS;
}

void FAT12FileSystem::destroyVnode(Vnode* vnode)
{
    if (vnode->type == Vnode::DIRECTORY)
    {
        File* file = (File*)vnode->fnode;
        delete file;
    }
    else
    {
        Directory* directory = (Directory*)vnode->fnode;
        delete directory;
    }
    delete vnode;
}


/*----------------------------------------------------------------------
    private functions
----------------------------------------------------------------------*/
int FAT12FileSystem::deviceOn()
{
    fd_->motor(ON);
    fd_->recalibrate();
    fd_->recalibrate();
    fd_->recalibrate();
    return MONA_SUCCESS;
}

int FAT12FileSystem::deviceOff()
{
    fd_->motorAutoOff();
    return 0;
}

Directory* FAT12FileSystem::searchFile(char* path, int* entry, int* cursor)
{
    Directory *p = current_;
    int index = -1;

    for (int i = 0; '\0' != path[i]; i++) {
        if ('/' == path[i])
            index = i;
    }


    *cursor = 0;

    if (-1 != index) {
        path[index] = '\0';

        char *dir = path;
        if (0 == index)
            dir = "/";

        int tmp = 0;

        p = trackingDirectory(dir, &tmp);
        if (NULL == p) {
            return NULL;
        }

        if ('\0' != dir[tmp]) {
            freeDirectory(p);
            return NULL;
        }

        *cursor = index + 1;
    }

    *entry = p->searchEntry((byte*)path+*cursor);
    return p;
}

Directory* FAT12FileSystem::trackingDirectory(char *path, int *cursor)
{
    Directory *p = current_;
    int i = *cursor;
    int j;

    if ('/' == path[i]) {
        p = fatroot_;
        i++;
    }

    while ('\0' != path[i]) {
        for (j = i; '\0' != path[j]; j++)
            if ('/' == path[j])
                break;
        int next = j + 1;
        if ('\0' == path[j])
            next = j;
        path[j] = '\0';

        char *name = path+i;

        if (0 == strcmp(name, "."))
            name = "..";
        else if (0 == strcmp(name, ".."))
            name = "...";

        int entry = p->searchEntry((byte*)name);

        if (j != next)
            path[j] = '/';

        if (-1 == entry)
            break;

        Directory *tmp = p->getDirectory(entry);

        if (NULL == tmp) {
            freeDirectory(p);
            return NULL;
        }

        freeDirectory(p);
        p = tmp;

        i = next;
    }

    *cursor = i;

    return p;
}


void FAT12FileSystem::freeDirectory(Directory *p)
{
    if (p->getIdentifer() == current_->getIdentifer())
    {
        if (p != current_)
        {
            delete current_;
            current_ = p;
        }
    }
    else
    {
        delete p;
    }
}


int FAT12FileSystem::readdirInternal(char* name, int* size, int* attribute)
{
    *attribute = 0;
    if (lsinfo_.entry == -1)
    {
// don't free, we reuse vnode->fnode
//        if (lsinfo_.p != current_) freeDirectory(lsinfo_.p);
        return MONA_FAILURE;
    }

    if (-1 == lsinfo_.p->getEntryName(lsinfo_.entry, (byte*)name))
    {
// don't free, we reuse vnode->fnode
//        freeDirectory(lsinfo_.p);
        return MONA_FAILURE;
    }

    /* directory */
    if (lsinfo_.p->isDirectory(lsinfo_.entry))
    {
        *attribute |= ATTRIBUTE_DIRECTORY;
    }

    lsinfo_.entry = lsinfo_.p->getNextEntry(lsinfo_.entry);
    return MONA_SUCCESS;
}

