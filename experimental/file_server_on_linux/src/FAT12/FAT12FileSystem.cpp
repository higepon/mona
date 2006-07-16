#include "FAT12FileSystem.h"

using namespace std;
//using namespace fat12;

FAT12FileSystem::FAT12FileSystem(IStorageDevice* drive, VnodeManager* vmanager) : drive_(drive), vmanager_(vmanager)
{
}

FAT12FileSystem::~FAT12FileSystem()
{
}

/*----------------------------------------------------------------------
    public interfaces
----------------------------------------------------------------------*/
int FAT12FileSystem::initialize()
{
    return MONA_SUCCESS;
}

int FAT12FileSystem::lookup(Vnode* diretory, const string& file, Vnode** found, int type)
{
    return MONA_SUCCESS;
}

int FAT12FileSystem::open(Vnode* file, int mode)
{
    return MONA_SUCCESS;
}

int FAT12FileSystem::read(Vnode* file, struct io::Context* context)
{
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
    return MONA_SUCCESS;
}

Vnode* FAT12FileSystem::getRoot() const
{
    return root_;
}

int FAT12FileSystem::readdir(Vnode* dir, monapi_cmemoryinfo** entries)
{
    return MONA_SUCCESS;
}

void FAT12FileSystem::destroyVnode(Vnode* vnode)
{
}


/*----------------------------------------------------------------------
    private functions
----------------------------------------------------------------------*/

