#include "ISO9660FileSystem.h"
#include <monapi.h>
#include "sys/error.h"
#include <monapi/Buffer.h>

using namespace std;
using namespace iso9660;
using namespace MonAPI;

extern string upperCase(const string& s);

ISO9660FileSystem::ISO9660FileSystem(IStorageDevice* drive) : drive_(drive), isJoliet_(false)
{
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
    int ret = readVolumeDescriptor();
    if (ret != M_OK) {
        monapi_warn("read volume descriptor error%s %s:%d\n", __func__, __FILE__, __LINE__);
        return ret;
    }

    ret = setDirectoryCache();
    if (ret != M_OK) {
        return ret;
    }

    root_ = new Vnode;
    root_->fnode  = rootDirectory_;
    root_->fs     = this;
    root_->type = Vnode::DIRECTORY;
    return M_OK;
}

int ISO9660FileSystem::lookup(Vnode* diretory, const string& file, Vnode** found, int type)
{
    Entry* directoryEntry = (Entry*)diretory->fnode;
    Entry* target = NULL;

    if (type == Vnode::REGULAR) {
        target = lookupFile(directoryEntry, file);
    } else {
        target = lookupDirectory(directoryEntry, file);
    }
    if (target == NULL) return M_FILE_NOT_FOUND;
    Vnode* newVnode = new Vnode;
    newVnode->fnode  = target;
    newVnode->type = type;
    newVnode->fs = this;
    *found = newVnode;
    return M_OK;
}

int ISO9660FileSystem::open(Vnode* file, intptr_t mode)
{
    return M_OK;
}

int ISO9660FileSystem::create(Vnode* dir, const string& file)
{
    return M_WRITE_ERROR;
}

int ISO9660FileSystem::write(Vnode* file, struct io::Context* context)
{
    return M_WRITE_ERROR;
}

int ISO9660FileSystem::read(Vnode* file, struct io::Context* context)
{
    Entry* fileEntry = (Entry*)file->fnode;
    uint32_t offset = context->offset;
    uint32_t readSize = context->size;
    uint32_t rest = fileEntry->attribute.size - offset;
    if (rest == 0) {
        return M_READ_ERROR;
    }

    if (rest < readSize) {
        readSize = rest;
    }

    int lba = fileEntry->attribute.extent + offset / SECTOR_SIZE;
    int sectorCount = (offset + readSize + SECTOR_SIZE - 1) / SECTOR_SIZE - offset / SECTOR_SIZE;
    uint32_t sectorSize = sectorCount * SECTOR_SIZE;

    int dataOffset = offset - (lba - fileEntry->attribute.extent) * SECTOR_SIZE;
    context->memory = new SharedMemory(readSize);
    if (context->memory->map(true) != M_OK)
    {
        delete context->memory;
        return M_NO_MEMORY;
    }
    // by junjunn
    if (0 == dataOffset) {
// by higepon
//        bool readResult = drive_->read(lba, context->memory->data(), sectorSize) == 0;
        bool readResult = drive_->read(lba, context->memory->data(), readSize) == 0;
        if (!readResult) {
            return M_READ_ERROR;
        }
    } else {
        uint8_t* temp = new uint8_t[sectorSize];
        if (temp == NULL) return M_NO_MEMORY;
        MonAPI::Buffer tempBuf(temp, sectorSize);
        MonAPI::Buffer dest(context->memory->data(), context->memory->size());
        bool readResult = drive_->read(lba, temp, sectorSize) == 0;
        if (!readResult) {
            delete temp;
            return M_READ_ERROR;
        }
        bool isOK = MonAPI::Buffer::copy(dest, 0, tempBuf, dataOffset, readSize);
        ASSERT(isOK);
        delete[] temp;
    }
    context->resultSize = readSize;
    context->offset += readSize;
    return M_OK;
}

int ISO9660FileSystem::close(Vnode* file)
{
    return M_OK;
}

int ISO9660FileSystem::stat(Vnode* file, Stat* st)
{
    Entry* entry = (Entry*)file->fnode;
    st->size = entry->attribute.size;
    st->datetime.year = entry->createDate.year;
    st->datetime.month = entry->createDate.month;
    st->datetime.day = entry->createDate.day;
    st->datetime.hour = entry->createDate.hour;
    st->datetime.min = entry->createDate.min;
    st->datetime.sec = entry->createDate.sec;
    return M_OK;
}

Vnode* ISO9660FileSystem::getRoot() const
{
    return root_;
}

int ISO9660FileSystem::read_directory(Vnode* dir, SharedMemory** entries)
{
    Entry* directory = (Entry*)dir->fnode;
    setDetailInformation(directory);
    uint32_t readSize = ((uint32_t)((directory->attribute.size + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;
    uint8_t* buffer = readdirToBuffer(directory, readSize);
    if (buffer == NULL) return M_NO_MEMORY;
    EntryList entryList;
    for (uint32_t position = 0 ; position < readSize;)
    {
        DirectoryEntry* iEntry = (DirectoryEntry*)((uint32_t)buffer + position);

        if (iEntry->length == 0)
        {
            // check next sector
            position = ((position + SECTOR_SIZE - 1) / SECTOR_SIZE) * SECTOR_SIZE;
            continue;
        }
        Entry* entry = setupEntry(iEntry);
        entryList.push_back(entry);
        position += iEntry->length;
    }
    delete[] buffer;
    int size = entryList.size();
    SharedMemory* ret = new SharedMemory(sizeof(int) + size * sizeof(monapi_directoryinfo));
    if (ret->map(true) != M_OK)
    {
        delete ret;
        return M_NO_MEMORY;
    }
    memcpy(ret->data(), &size, sizeof(int));
    monapi_directoryinfo* p = (monapi_directoryinfo*)&ret->data()[sizeof(int)];
    for (EntryList::iterator i = entryList.begin(); i != entryList.end(); ++i)
    {
        monapi_directoryinfo di;

        di.size = (*i)->attribute.size;
        strcpy(di.name, (*i)->name.c_str());
        di.attr = (*i)->isDirectory ? ATTRIBUTE_DIRECTORY : 0;
        *p = di;
        p++;
        delete (*i);
    }
    *entries = ret;

    return M_OK;
}

/*----------------------------------------------------------------------
    private functions
----------------------------------------------------------------------*/
// you should delete return value
uint8_t* ISO9660FileSystem::readdirToBuffer(Entry* directory, uint32_t readSize)
{
    uint8_t* buffer = new uint8_t[readSize];

    if (buffer == NULL)
    {
        return NULL;
    }
    bool readResult = drive_->read(directory->attribute.extent, buffer, readSize) == 0;

    if (!readResult)
    {
        delete buffer;
        return NULL;
    }
    return buffer;
}

bool ISO9660FileSystem::isJolietDescriptor(SupplementaryVolumeDescriptor* desc) const
{
    const char* UCS2_LEVEL1 = "%/@";
    const char* UCS2_LEVEL2 = "%/C";
    const char* UCS2_LEVEL3 = "%/E";

    if (strncmp(UCS2_LEVEL1, desc->escape_sequences, 3) == 0 ||
        strncmp(UCS2_LEVEL2, desc->escape_sequences, 3) == 0 ||
        strncmp(UCS2_LEVEL3, desc->escape_sequences, 3) == 0) {
        if ((desc->volume_flags[0] & 0x01) == 0) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

// you should delete return value
Entry* ISO9660FileSystem::setupEntry(DirectoryEntry* from)
{
    Entry* entry = new Entry;
    entry->isDirectory = from->directory == 1;
    entry->name = canonicalizeName(from->name, from->name_len);
    return entry;
}

int ISO9660FileSystem::readVolumeDescriptor()
{
    int i;
    bool primaryVolumeDescriptorFound = false;

    BaseVolumeDescriptor* descriptor = new BaseVolumeDescriptor;
    if (descriptor == NULL)
    {
        monapi_fatal("BaseVolumeDescriptor allocate error%s %s:%d\n", __func__, __FILE__, __LINE__);
        return M_NO_MEMORY;
    }
    for (i = 16; i < 100; i++)
    {
        // read
        bool readResult = drive_->read(i, descriptor, SECTOR_SIZE) == 0;
        if (!readResult)
        {
            delete descriptor;
            monapi_fatal("device read error%s %s:%d\n", __func__, __FILE__, __LINE__);
            return M_READ_ERROR;
        }
        // read primary descriptor
        if (descriptor->type == ISO_PRIMARY_VOLUME_DESCRIPTOR && strncmp("CD001", descriptor->id, 5) == 0)
        {
            PrimaryVolumeDescriptor* p = (PrimaryVolumeDescriptor*)(descriptor);

            // keep primary descriptor
            pdescriptor_ = *p;
            primaryVolumeDescriptorFound = true;
        }
        else if (descriptor->type == ISO_SUPPLEMENTARY_VOLUME_DESCRIPTOR && strncmp("CD001", descriptor->id, 5) == 0)
        {
            SupplementaryVolumeDescriptor* s = (SupplementaryVolumeDescriptor*)(descriptor);
            if (isJolietDescriptor(s)) {
                isJoliet_ = true;
                pdescriptor_ = *(PrimaryVolumeDescriptor*)s;
                break;
            }
        } else if (descriptor->type == ISO_END_VOLUME_DESCRIPTOR) {
            break;
        }
    }
    // invalid
    if (i == 100 || !primaryVolumeDescriptorFound)
    {
        monapi_fatal("BaseVolumeDescriptor allocate error%s %s:%d\n", __func__, __FILE__, __LINE__);
        return M_READ_ERROR;
    }

    delete descriptor;
    return M_OK;
}

uint8_t* ISO9660FileSystem::readPathTableIntoBuffer()
{
    uint32_t readSize = ((uint32_t)((pdescriptor_.path_table_size_l + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;
    uint8_t* buffer = new uint8_t[readSize];

    if (buffer == NULL)
    {
        return NULL;
    }

    bool readResult = drive_->read(pdescriptor_.type_l_path_table, buffer, readSize) == 0;

    return readResult ? buffer : NULL;
}

int ISO9660FileSystem::setDirectoryCache()
{
     uint8_t* buffer;

    // read path table
    if ((buffer = readPathTableIntoBuffer()) == NULL)
    {
        return M_NO_MEMORY;
    }

    // create DirectoryEntries from path table
    EntryList directoryList;

    createDirectoryListFromPathTable(&directoryList, buffer);
    delete[] buffer;

    // directory not found
    if (directoryList.size() == 0)
    {
        return M_FILE_NOT_FOUND;
    }

    // set up root direcotyr
    rootDirectory_ = directoryList[0];

    DirectoryEntry* rootEntry = (DirectoryEntry*)((pdescriptor_.root_directory_record));
    setDetailInformation(rootDirectory_, rootEntry);

    // set children
    for (uint32_t i = 0; i < directoryList.size(); i++)
    {
        setDirectoryRelation(&directoryList, directoryList[i]);
    }

    // parent of root is root
    directoryList[0]->parent = directoryList[0];

    return M_OK;
}

void ISO9660FileSystem::createDirectoryListFromPathTable(EntryList* list, uint8_t* buffer)
{
    for (uint32_t id = 1, position = 0; position < pdescriptor_.path_table_size_l; id++)
    {
        Entry* entry = new Entry;

        PathTableEntry* pathEntry = (PathTableEntry*)(buffer + position);
        entry->attribute.id       = id;
        entry->attribute.extent   = pathEntry->extent;
        entry->attribute.parentID = pathEntry->parentDirectory;

        entry->name = canonicalizeName(pathEntry->name, pathEntry->length);
        list->push_back(entry);

        /* next path table entry */
        position += pathEntry->length + sizeof(PathTableEntry) + (pathEntry->length % 2 ? 1 : 0);
    }
}

string ISO9660FileSystem::canonicalizeName(const char* name, int nameLen)
{
    if (nameLen == 1 && name[0] == 0x00) {
        return ".";
    } else if (nameLen == 1 && name[0] == 0x01) {
        return "..";
    }


    if (isJoliet_) {
        return upperCase(nameToUtf8(name, nameLen));
    } else {
        string ret = string(name, nameLen);
        // ISO 9660 Level1
        string::size_type index;
        if ((index = ret.find(';')) != string::npos) {
            ret = ret.substr(0, index);
        }

        // necessary?
        if (ret[ret.size() - 1] == '.') {
            ret = ret.substr(0, ret.size() - 1);
        }
        return upperCase(ret);
    }
}

string ISO9660FileSystem::nameToUtf8(const char* name, int nameSizeByte)
{
    if (isJoliet_) {
        string ret;
        // On Joliet, name are UCS2.
        for (int i = 0; i < nameSizeByte; i += 2) {
            uint16_t ch =  (name[i] << 8) | (name[i + 1]);
            uint8_t buf[4];
            int len = MonAPI::Encoding::ucs4ToUtf8(ch, buf);
            for (int j = 0; j < len; j++) {
                ret += buf[j];
            }
        }
        return ret;
    } else {
        return string(name, nameSizeByte);
    }
}

void ISO9660FileSystem::setDetailInformation(Entry* to, DirectoryEntry* from)
{
    KDate& createDate = to->createDate;

    to->name = canonicalizeName(from->name, from->name_len);
    to->attribute.extent= from->extent_l;
    to->attribute.size  = from->size_l;
    createDate.year = from->date[0] + 1900;
    createDate.month = from->date[1];
    createDate.day = from->date[2];
    createDate.hour = from->date[3];
    createDate.min = from->date[4];
    createDate.sec = from->date[5];
    to->hasDetail = true;
}

void ISO9660FileSystem::setDirectoryRelation(EntryList* list, Entry* directory)
{
    uint32_t self = directory->attribute.id;

    for (uint32_t i = 0; i < list->size(); i++)
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

void ISO9660FileSystem::split(string str, char ch, vector<string>& v)
{
    uint32_t index = 0;
    uint32_t next = 0;
    while ((index = str.find_first_of(ch, next)) != string::npos)
    {
        v.push_back(string(str.begin() + next, str.begin() + index));
        next = index + 1;
    }
    v.push_back(string(str.begin() + next, str.end()));
}

Entry* ISO9660FileSystem::lookupDirectory(Entry* root, const string& path)
{
    bool found;
    vector<string> elements;
    split(path, '/', elements);

    for (vector<string>::iterator element = elements.begin(); element != elements.end(); ++element)
    {
        if (*element == ".")
        {
            continue;
        }
        else if (*element == "..")
        {
            root = root->parent;
            continue;
        }
        else if (*element == "")// || *element == NULL)
        {
            continue;
        }

        found = false;
        for (EntryList::iterator i = root->children.begin(); i != root->children.end(); ++i)
        {
            if ((*i)->name == *element)
            {
                root = *i;
                found = true;
                break;
            }
        }
        if (!found) return NULL;
    }
    return root;
}

// return value should be delete, when close
Entry* ISO9660FileSystem::lookupFile(Entry* directory, const string& fileName)
{
    setDetailInformation(directory);
    uint32_t readSize = ((uint32_t)((directory->attribute.size + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;
    uint8_t* buffer = new uint8_t[readSize];
    if (buffer == NULL)
    {
        return NULL;
    }

    bool readResult = drive_->read(directory->attribute.extent, buffer, readSize) == 0;
    if (!readResult)
    {
        delete buffer;
        return NULL;
    }

    for (uint32_t position = 0; position < readSize;)
    {
        DirectoryEntry* iEntry = (DirectoryEntry*)(buffer + position);

        if (iEntry->length == 0)
        {
            // check next sector
            position = ((position + SECTOR_SIZE - 1) / SECTOR_SIZE) * SECTOR_SIZE;
            continue;
        }
        string name = canonicalizeName(iEntry->name, iEntry->name_len);
        if (iEntry->directory == 0 && fileName == name)
        {
            Entry* foundFile = new Entry;

            setDetailInformation(foundFile, iEntry);

            delete[] buffer;
            return foundFile;
        }

        position += iEntry->length;
        iEntry = (DirectoryEntry*)(buffer + position);
    }
    delete[] buffer;
    return NULL;
}

bool ISO9660FileSystem::setDetailInformation(Entry* entry)
{
    if (entry->hasDetail) return true;

    if (!(entry->parent->hasDetail))
    {
        setDetailInformation(entry->parent);
    }

    uint32_t readSize = ((uint32_t)((entry->parent->attribute.size + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;
    uint8_t* buffer = new uint8_t[readSize];

    if (buffer == NULL)
    {
        return false;
    }

    bool readResult = drive_->read(entry->parent->attribute.extent, buffer, readSize) == 0;

    if (!readResult)
    {
        delete[] buffer;
        return false;
    }

    EntryList* children = &(entry->parent->children);

    for (uint32_t position = 0; position < readSize;)
    {
        DirectoryEntry* iEntry = (DirectoryEntry*)(buffer + position);
        string name =canonicalizeName(iEntry->name, iEntry->name_len);

        if (iEntry->length == 0)
        {
            position = ((position + SECTOR_SIZE - 1) / SECTOR_SIZE) * SECTOR_SIZE;
            continue;
        }
        for (EntryList::iterator i = children->begin(); i != children->end(); ++i)
        {
            Entry* child = *i;
            if (name != child->name) continue;

            setDetailInformation(child, iEntry);
        }

        position += iEntry->length;
    }
    delete[] buffer;
    return true;
}

void ISO9660FileSystem::destroy_vnode(Vnode* vnode)
{
    iso9660::Entry* entry = (iso9660::Entry*)vnode->fnode;
    if (vnode->type != Vnode::DIRECTORY) delete entry; // directory is deleted on destructor
    delete vnode;
}
