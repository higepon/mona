#include "ISO9660FileSystem.h"

using namespace std;
using namespace iso9660;

ISO9660FileSystem::ISO9660FileSystem(IStorageDevice* drive, VnodeManager* vmanager)
{
    drive_ = drive;
    vmanager_ = vmanager;
}

ISO9660FileSystem::~ISO9660FileSystem()
{

}

/*----------------------------------------------------------------------
    public interfaces
----------------------------------------------------------------------*/
int ISO9660FileSystem::initialize()
{
    if (readVolumeDescriptor() != MONA_SUCCESS)
    {
        printf("read volume descriptor error%s %s:%d\n", __func__, __FILE__, __LINE__);
        return MONA_SUCCESS;
    }
//     if (!SetDirectoryCache())
//     {
//         printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
//         return false;
//     }

//     root_ = vmanager_->alloc();
//     root_->fnode  = rootDirectory;
//     root_->fs     = this;
//     root_->type = Vnode::DIRECTORY;
//     return true;

    return MONA_SUCCESS;
}

int ISO9660FileSystem::lookup(Vnode* diretory, const string& file, Vnode** found, int type)
{
    return MONA_SUCCESS;
}

int ISO9660FileSystem::open(Vnode* file, int mode)
{
    return MONA_SUCCESS;
}

int ISO9660FileSystem::read(Vnode* file, struct io::Context* context)
{
    return MONA_SUCCESS;
}

int ISO9660FileSystem::seek(Vnode* file, dword offset, dword origin)
{
    return MONA_SUCCESS;
}

int ISO9660FileSystem::close(Vnode* file)
{
    return MONA_SUCCESS;
}

/*----------------------------------------------------------------------
    private functions
----------------------------------------------------------------------*/
int ISO9660FileSystem::readVolumeDescriptor()
{
    int i;
    bool primaryVolumeDescriptorFound = false;

    BaseVolumeDescriptor* descriptor = new BaseVolumeDescriptor;
    if (descriptor == NULL)
    {
        printf("BaseVolumeDescriptor allocate error%s %s:%d\n", __func__, __FILE__, __LINE__);
        return MONA_FAILURE;
    }
    for (i = 16; i < 100; i++)
    {
        // read
        bool readResult = drive_->read(i, descriptor, SECTOR_SIZE) == 0;
        if (!readResult)
        {
            delete descriptor;
            printf("device read error%s %s:%d\n", __func__, __FILE__, __LINE__);
            return MONA_FAILURE;
        }
        // read primary descriptor
        if (descriptor->type == ISO_PRIMARY_VOLUME_DESCRIPTOR && strncmp("CD001", descriptor->id, 5) == 0)
        {
            PrimaryVolumeDescriptor* p = (PrimaryVolumeDescriptor*)(descriptor);

            // keep primary descriptor
            pdescriptor_ = *p;
            primaryVolumeDescriptorFound = true;
        }

        // end
        if (descriptor->type == ISO_END_VOLUME_DESCRIPTOR)
        {
            break;
        }
    }
    // invalid
    if (i == 100 || !primaryVolumeDescriptorFound)
    {
        printf("BaseVolumeDescriptor allocate error%s %s:%d\n", __func__, __FILE__, __LINE__);
        return MONA_FAILURE;
    }

    delete descriptor;
    return MONA_SUCCESS;
}

byte* ISO9660FileSystem::readPathTableIntoBuffer()
{
    dword readSize = ((dword)((pdescriptor_.path_table_size_l + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;
    byte* buffer = new byte[readSize];

    if (buffer == NULL)
    {
        return NULL;
    }

    bool readResult = drive_->read(pdescriptor_.type_l_path_table, buffer, readSize) == 0;

    return readResult ? buffer : NULL;
}

int ISO9660FileSystem::setDirectoryCache()
{
     byte* buffer;

    // read path table
    if ((buffer = readPathTableIntoBuffer()) == NULL)
    {
        return MONA_FAILURE;
    }

//     // create DirectoryEntries from path table
//     HList<ISO9660Directory*> directoryList;

//     CreateDirectoryListFromPathTable(&directoryList, buffer);
//     delete[] buffer;

//     // directory not found
//     if (directoryList.size() == 0)
//     {
//         this->lastError = DIRECTORY_NOT_FOUND;
//         return MONA_FAILURE;
//     }

//     // set up root direcotyr
//     this->rootDirectory = directoryList[0];

//     ISODirectoryEntry* rootEntry = (ISODirectoryEntry*)((this->pdescriptor.root_directory_record));
//     SetDetailInformation(this->rootDirectory, rootEntry);

//     // set children
//     for (int i = 0; i < directoryList.size(); i++)
//     {
//         SetDirectoryRelation(&directoryList, directoryList[i]);
//     }

//     // parent of root is root
//     directoryList[0]->parent = directoryList[0];

    return MONA_SUCCESS;
}
