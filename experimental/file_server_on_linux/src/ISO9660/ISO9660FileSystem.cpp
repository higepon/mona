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
    deleteEntry(rootDirectory_);
}

/*----------------------------------------------------------------------
    public interfaces
----------------------------------------------------------------------*/
int ISO9660FileSystem::initialize()
{
    if (readVolumeDescriptor() != MONA_SUCCESS)
    {
        printf("read volume descriptor error%s %s:%d\n", __func__, __FILE__, __LINE__);
        return MONA_FAILURE;
    }

    if (setDirectoryCache() != MONA_SUCCESS)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        return MONA_FAILURE;
    }

    root_ = vmanager_->alloc();
    root_->fnode  = rootDirectory_;
    root_->fs     = this;
    root_->type = Vnode::DIRECTORY;
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
    Entry* fileEntry = (Entry*)file->fnode;
    monapi_cmemoryinfo* memory = context->memory;
    int readSize = context->size >= memory->Size ? memory->Size : context->size;
    dword offset = context->offset;
    dword rest = fileEntry->attribute.size - offset;

    if (rest < readSize)
    {
        readSize = rest;
    }

    int lba = fileEntry->attribute.extent + offset / SECTOR_SIZE;
    int sectorCount = (offset + readSize + SECTOR_SIZE - 1) / SECTOR_SIZE - offset / SECTOR_SIZE;
    dword sectorSize = sectorCount * SECTOR_SIZE;

    byte* temp = new byte[sectorSize];
    if (temp == NULL) return MONA_FAILURE;

    bool readResult = drive_->read(lba, temp, readSize);
    if (!readResult)
    {
        delete temp;
        return MONA_FAILURE;
    }

    memcpy(memory->Data, temp + offset -(lba - fileEntry->attribute.extent) * SECTOR_SIZE, readSize);
    delete[] temp;
    context->resultSize = readSize;
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

    // create DirectoryEntries from path table
    EntryList directoryList;

    createDirectoryListFromPathTable(&directoryList, buffer);
    delete[] buffer;

    // directory not found
    if (directoryList.size() == 0)
    {
        return MONA_FAILURE;
    }

    // set up root direcotyr
    rootDirectory_ = directoryList[0];

    DirectoryEntry* rootEntry = (DirectoryEntry*)((pdescriptor_.root_directory_record));
    setDetailInformation(rootDirectory_, rootEntry);

    // set children
    for (dword i = 0; i < directoryList.size(); i++)
    {
        setDirectoryRelation(&directoryList, directoryList[i]);
    }

    // parent of root is root
    directoryList[0]->parent = directoryList[0];

    return MONA_SUCCESS;
}

void ISO9660FileSystem::createDirectoryListFromPathTable(EntryList* list, byte* buffer)
{
    for (dword id = 1, position = 0; position < pdescriptor_.path_table_size_l; id++)
    {
        Entry* entry = new Entry;

        PathTableEntry* pathEntry = (PathTableEntry*)(buffer + position);
        entry->attribute.id       = id;
        entry->attribute.extent   = pathEntry->extent;
        entry->attribute.parentID = pathEntry->parentDirectory;

        if (pathEntry->length == 1 && pathEntry->name[0] == 0x00)
        {
            entry->name = ".";
        }
        else if (pathEntry->length == 1 && pathEntry->name[0] == 0x01)
        {
            entry->name = "..";
        }
        else
        {
            entry->name = string(pathEntry->name, pathEntry->length);
        }

        list->push_back(entry);

        /* next path table entry */
        position += pathEntry->length + sizeof(PathTableEntry) + (pathEntry->length % 2 ? 1 : 0);
    }
}

void ISO9660FileSystem::setDetailInformation(Entry* to, DirectoryEntry* from)
{
    FileDate* createDate = &(to->createDate);

    to->name = (getProperName(string(from->name, from->name_len)));

    to->attribute.extent= from->extent_l;
    to->attribute.size  = from->size_l;
    createDate->setYear(from->date[0] + 1900);
    createDate->setMonth(from->date[1]);
    createDate->setDay(from->date[2]);
    createDate->setHour(from->date[3]);
    createDate->setMinute(from->date[4]);
    createDate->setSecond(from->date[5]);
    to->hasDetail = true;
}

string ISO9660FileSystem::getProperName(const string& name)
{
    string result = name;

    if (result[0] == 0x00)
    {
        result = ".";
    }
    else if (result[0] == 0x01)
    {
        result = "..";
    }

    if (name.find(';') != string::npos)
    {
        result = result.substr(0, result.find(";"));
    }

    if (result[result.size() - 1] == '.' && result != "." && result != "..")
    {
        result = result.substr(0, result.size() - 1);
    }
    return result;
}

void ISO9660FileSystem::setDirectoryRelation(EntryList* list, Entry* directory)
{
    dword self = directory->attribute.id;

    for (dword i = 0; i < list->size(); i++)
    {
        Entry* entry = list->at(i);

        if (entry->attribute.parentID != self) continue;
        if (i == 0 && entry->attribute.parentID == 1) continue;

        directory->children.push_back(entry);
        entry->parent = directory;
    }
}

void ISO9660FileSystem::deleteEntry(Entry* entry)
{
    if (entry == NULL) return;
    EntryList* children = &(entry->children);
    for (EntryList::iterator i = children->begin(); i != children->end(); ++i )
    {
        deleteEntry(*i);
    }
    delete entry;
}

