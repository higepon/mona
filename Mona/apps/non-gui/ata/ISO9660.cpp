/*!
    \file  iso9660.cpp
    \brief iso9660

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2004/11/09 update:$Date$
*/

#include <monapi/string.h>
#include "iso9660.h"
#include <monapi.h>

using namespace MonAPI;

/*----------------------------------------------------------------------
    ISO9660File
----------------------------------------------------------------------*/
ISO9660File::ISO9660File(ISO9660* iso, DirectoryEntry* fileEntry) : iso(iso), fileEntry(fileEntry), pointer(0)
{
}

ISO9660File::~ISO9660File()
{
    delete fileEntry;
}

dword ISO9660File::Read(void* buffer, dword size)
{
    dword rest = this->fileEntry->size - this->pointer;

    if (rest < size)
    {
        size = rest;
    }

    int lba = fileEntry->extent + (this->pointer + ISO9660::SECTOR_SIZE - 1) / ISO9660::SECTOR_SIZE;
    int sectorCount = (this->pointer + size + ISO9660::SECTOR_SIZE - 1) / ISO9660::SECTOR_SIZE - this->pointer / ISO9660::SECTOR_SIZE;
    dword readSize = sectorCount * ISO9660::SECTOR_SIZE;

    byte* temp = new byte[readSize];
    if (temp == NULL) return 0;

    bool readResult = iso->Read(lba, temp, readSize);
    if (!readResult)
    {
        delete temp;
        return 0;
    }

    memcpy(buffer, temp + (this->pointer - ((int)this->pointer / ISO9660::SECTOR_SIZE)) * ISO9660::SECTOR_SIZE, size);

    delete temp;
    return size;
}

bool ISO9660File::Seek(int position, int flag)
{
    if (SEEK_SET == flag)
    {
        if (position < 0)
        {
            this->pointer = 0;
        }
        else
        {
            this->pointer = position;
        }
    }
    else if (SEEK_CUR == flag)
    {
        this->pointer += pointer;
        if (0 > position)
        {
            if (this->pointer < (dword)-position)
            {
                this->pointer = 0;
            }
        }
    }
    else if (SEEK_END == flag)
    {
        this->pointer = fileEntry->size + pointer;
        if (0 > pointer)
        {
            if (fileEntry->size < (dword)-position)
            {
                this->pointer = 0;
            }
        }
    }

    if (this->pointer > this->fileEntry->size) this->pointer = this->fileEntry->size;

    return true;
}

/*----------------------------------------------------------------------
    ISO9660
----------------------------------------------------------------------*/
ISO9660::ISO9660(IStorageDevice* device, const CString& mountPath)
{
    this->device    = device;
    this->mountPath = mountPath;
    this->lastError = 0;
    this->primaryVolumeDescriptorFound = false;
}

ISO9660::~ISO9660()
{
    DeleteDirectoryEntry(this->rootDirectory);
}

void ISO9660::DeleteDirectoryEntry(DirectoryEntry* entry)
{
    for (int i = 0; i < entry->children.size(); i++)
    {
        DeleteDirectoryEntry(entry->children[i]);
    }

    delete entry;
}

bool ISO9660::Initialize()
{
    if (!ReadVolumeDescriptor())
    {
        return false;
    }

    if (!SetDirectoryCache())
    {
        return false;
    }

    return true;
}

File* ISO9660::GetFile(const CString& path)
{
    int lastIndexOfSlash = path.lastIndexOf('/');

    if (lastIndexOfSlash == -1) return NULL;

    CString directoryPath = path.substring(0, lastIndexOfSlash);
    CString fileName      = path.substring(lastIndexOfSlash + 1, path.getLength() - lastIndexOfSlash);

    DirectoryEntry* directoryEntry = FindDirectoryEntry(this->rootDirectory, directoryPath);
    if (directoryEntry == NULL) return NULL;

    DirectoryEntry* fileEntry = FindFileEntry(directoryEntry, fileName);
    if (fileEntry == NULL) return NULL;

    File* file = new ISO9660File(this, fileEntry);

    file->year  = fileEntry->year;
    file->month = fileEntry->month;
    file->day   = fileEntry->day;
    file->hour  = fileEntry->hour;
    file->min   = fileEntry->min;
    file->sec   = fileEntry->sec;

    return file;
}

bool ISO9660::Read(dword lba, byte* buffer, dword size)
{
    return this->device->read(lba, buffer, size) == 0;
}

bool ISO9660::ReadVolumeDescriptor()
{
    int i;

    ISOBaseVolumeDescriptor* descriptor = new ISOBaseVolumeDescriptor;

#if 0
    Log("sizeof(ISOBaseVolumeDescriptor)=%d\n", sizeof(ISOBaseVolumeDescriptor));

    dword *p = (dword*)descriptor;
    p = (dword*)((byte*)p + sizeof(ISOBaseVolumeDescriptor));
    *p = 0x12345678;

    Log("*p=%x\n", *p);
#endif

    if (descriptor == NULL)
    {
        this->lastError = MEMORY_ALLOCATE_ERROR;
        return false;
    }

    for (i = 16; i < 100; i++)
    {
        /* read */
        bool readResult = this->device->read(i, descriptor, SECTOR_SIZE) == 0;

#if 0
    Log("*p=%x\n", *p);
#endif

        if (!readResult)
        {
            this->lastError = READ_ERROR;
            delete descriptor;
            return false;
        }

        /* read primary descriptor */
        if (descriptor->type == ISO_PRIMARY_VOLUME_DESCRIPTOR && strncmp("CD001", descriptor->id, 5) == 0)
        {
            ISOPrimaryVolumeDescriptor* p = (ISOPrimaryVolumeDescriptor*)(descriptor);

            /* keep primary descriptor */
            this->pdescriptor = *p;

#define DEBUG_READ_TRACE
#ifdef DEBUG_READ_TRACE
 logprintf("logical_block_size_l;=%d\n", pdescriptor.logical_block_size_l);
 logprintf("path_table_size_l;=%d\n", pdescriptor.path_table_size_l);
 logprintf("   type_l_path_table;=%d\n",    pdescriptor.type_l_path_table);
#endif

            this->primaryVolumeDescriptorFound = true;
        }

        /* end */
        if (descriptor->type == ISO_END_VOLUME_DESCRIPTOR)
        {
            break;
        }
    }

    /* invalid */
    if (i == 100 || !primaryVolumeDescriptorFound)
    {
        this->lastError = VOLUME_DESCRIPTOR_NOT_FOUND;
        delete descriptor;
        return false;
    }

    delete descriptor;
    return true;
}

bool ISO9660::SetDirectoryCache()
{
    byte* buffer;

    /* read path table */
    if ((buffer = ReadPathTableIntoBuffer()) == NULL)
    {
        return false;
    }

    /* create DirectoryEntries from path table */
    HList<DirectoryEntry*> directoryList;

    CreateDirectoryListFromPathTable(&directoryList, buffer);
    delete buffer;

    /* directory not found */
    if (directoryList.size() == 0)
    {
        this->lastError = DIRECTORY_NOT_FOUND;
        return false;
    }

    /* set up root direcotyr */
    this->rootDirectory = directoryList[0];

    ISODirectoryEntry* rootEntry = (ISODirectoryEntry*)((this->pdescriptor.root_directory_record));
    SetDetailInformation(this->rootDirectory, rootEntry);

    /* set children */
    for (int i = 0; i < directoryList.size(); i++)
    {
        SetDirectoryRelation(&directoryList, directoryList[i]);
    }

    return true;
}

void ISO9660::SetDirectoryRelation(HList<DirectoryEntry*>* list, DirectoryEntry* directory)
{
    dword self = directory->id;

    for (int i = 0; i < list->size(); i++)
    {
        DirectoryEntry* entry = list->get(i);

        if (entry->parentID != self) continue;
        if (i == 0 && entry->parentID == 1) continue;

        directory->children.add(entry);
        entry->parent = directory;
    }
}

void ISO9660::CreateDirectoryListFromPathTable(HList<DirectoryEntry*>* list, byte* buffer)
{


    for (dword id = 1, position = 0; position < this->pdescriptor.path_table_size_l; id++)
    {
        DirectoryEntry* entry = new DirectoryEntry;

        ISOPathTableEntry* pathEntry = (ISOPathTableEntry*)(buffer + position);
        entry->id       = id;
        entry->extent   = pathEntry->extent;
        entry->parentID = pathEntry->parentDirectory;

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
            entry->name = CString(pathEntry->name, pathEntry->length);
        }

        list->add(entry);

        /* next path table entry */
        position += pathEntry->length + sizeof(ISOPathTableEntry) + (pathEntry->length % 2 ? 1 : 0);
    }
}

byte* ISO9660::ReadPathTableIntoBuffer()
{
    dword readSize = ((dword)((this->pdescriptor.path_table_size_l + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;
    byte* buffer = new byte[readSize];

    if (buffer == NULL)
    {
        this->lastError = MEMORY_ALLOCATE_ERROR;
        return NULL;
    }

    bool readResult = this->device->read(pdescriptor.type_l_path_table, buffer, readSize) == 0;

    return readResult ? buffer : NULL;
}

// for DEBUG
void ISO9660::PrintDirectory(DirectoryEntry* entry)
{
    printf("%s/", (const char*)(entry->name));

    if (entry->children.size() == 0)
    {
        printf("\n");
        return;
    }

    for (int i = 0; i < entry->children.size(); i++)
    {
        PrintDirectory(entry->children[i]);
    }
}

void ISO9660::SetDetailInformation(DirectoryEntry* to, ISODirectoryEntry* from)
{
    to->name = GetProperName(CString(from->name, from->name_len));

    to->extent= from->extent_l;
    to->size  = from->size_l;
    to->year  = from->date[0] + 1900;
    to->month = from->date[1];
    to->day   = from->date[2];
    to->hour  = from->date[3];
    to->min   = from->date[4];
    to->sec   = from->date[5];
    to->isDirectory = from->directory == 1;
    to->hasDetail = true;
}

CString ISO9660::GetProperName(const CString& name)
{
    if (name.indexOf(';') == -1) return name;
    return name.split(';')[0];
}

bool ISO9660::SetDetailInformation(DirectoryEntry* entry)
{
    if (entry->hasDetail) return true;

    if (!(entry->parent->hasDetail))
    {
        SetDetailInformation(entry->parent);
    }

    dword readSize = ((dword)((entry->parent->size + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;
    byte* buffer = new byte[readSize];

    if (buffer == NULL)
    {
        this->lastError = MEMORY_ALLOCATE_ERROR;
        return false;
    }

    bool readResult = this->device->read(entry->parent->extent, buffer, readSize) == 0;

    if (!readResult)
    {
        this->lastError = READ_ERROR;
        delete buffer;
        return false;
    }

    HList<DirectoryEntry*>* children = &(entry->parent->children);

    for (dword position = 0; position < readSize;)
    {
        ISODirectoryEntry* iEntry = (ISODirectoryEntry*)(buffer + position);
        CString name = CString(iEntry->name, iEntry->name_len);

        if (iEntry->length == 0) break;

        for (int i = 0; i < children->size(); i++)
        {
            DirectoryEntry* child = children->get(i);
            if (name != child->name) continue;

            SetDetailInformation(child, iEntry);
        }

        position += iEntry->length;
    }

    delete buffer;
    return true;
}

_A<CString> ISO9660::GetFileSystemEntries(const CString& path)
{
    DirectoryEntry* entry = FindDirectoryEntry(this->rootDirectory, path);
    if (entry == NULL) return NULL;


    SetDetailInformation(entry);
    dword readSize = ((dword)((entry->size + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;
    byte* buffer = new byte[readSize];

    if (buffer == NULL)
    {
        this->lastError = MEMORY_ALLOCATE_ERROR;
        return NULL;
    }

    bool readResult = this->device->read(entry->extent, buffer, readSize) == 0;

    if (!readResult)
    {
        this->lastError = READ_ERROR;
        delete buffer;
        return NULL;
    }

    HList<CString> names;
    for (dword position = 0, ; position < readSize;)
    {
        ISODirectoryEntry* iEntry = (ISODirectoryEntry*)(buffer + position);

        if (iEntry->length == 0) break;

        CString name;

        if (iEntry->name[0] == 0x00)
        {
            name = ".";
        }
        else if (iEntry->name[0] == 0x01)
        {
            name = "..";
        }
        else
        {
            name = GetProperName(CString(iEntry->name, iEntry->name_len));
        }

        names.add(name);
        position += iEntry->length;
    }
    delete buffer;

    _A<CString> ret(names.size());

    for (int i = 0; i < names.size(); i++)
    {
        ret[i] = names[i];
    }

    return ret;
}

DirectoryEntry* ISO9660::FindDirectoryEntry(DirectoryEntry* root, const CString& path)
{
    bool found;
    _A<CString> elements = path.split('/');

    FOREACH (CString, element, elements)
    {
        found = false;
        for (int i = 0; i < root->children.size(); i++)
        {
            if (root->children[i]->name == element)
            {
                root = root->children[i];
                found = true;
                break;
            }
        }
        if (!found) return NULL;
    }
    END_FOREACH

    return root;
}

// return value should be delete, when close
DirectoryEntry* ISO9660::FindFileEntry(DirectoryEntry* directory, const CString& fileName)
{
    SetDetailInformation(directory);

    dword readSize = ((dword)((directory->size + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;
    byte* buffer = new byte[readSize];

    if (buffer == NULL)
    {
        this->lastError = MEMORY_ALLOCATE_ERROR;
        return NULL;
    }

    bool readResult = this->device->read(directory->extent, buffer, readSize) == 0;

    if (!readResult)
    {
        this->lastError = READ_ERROR;
        delete buffer;
        return NULL;
    }

    for (dword position = 0; position < readSize;)
    {
        ISODirectoryEntry* iEntry = (ISODirectoryEntry*)(buffer + position);
        if (iEntry->length == 0) break;

        CString name = GetProperName(CString(iEntry->name, iEntry->name_len));

        if (iEntry->directory == 0 && fileName == name)
        {
            DirectoryEntry* foundFile = new DirectoryEntry;


            SetDetailInformation(foundFile, iEntry);

            delete buffer;
            return foundFile;
        }

        position += iEntry->length;
        iEntry = (ISODirectoryEntry*)(buffer + position);
    }

    delete buffer;
    return NULL;
}
