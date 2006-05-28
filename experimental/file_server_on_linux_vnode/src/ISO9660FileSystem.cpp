/*!
    \file  ISO9660FileSystem.cpp
    \brief ISO9660FileSystem

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2005/01/26 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#ifdef ON_LINUX
#include <string.h>
#include <stdio.h>
#else
#include <monapi/string.h>
#endif
#include "ISO9660FileSystem.h"
using namespace MonAPI;
using namespace std;

/*----------------------------------------------------------------------
    ISO9660FileSystem
----------------------------------------------------------------------*/
ISO9660FileSystem::ISO9660FileSystem(IStorageDevice* cd, VnodeCacher* cacher)
{
    this->cd            = cd;
    this->lastError     = NO_ERROR;
    this->rootDirectory = NULL;
    this->cacher_       = cacher;
}

ISO9660FileSystem::~ISO9660FileSystem()
{
    DeleteEntry(this->rootDirectory);
}

bool ISO9660FileSystem::Initialize()
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

bool ISO9660FileSystem::Read(dword lba, byte* buffer, dword size)
{
    return this->cd->read(lba, buffer, size) == 0;
}

File* ISO9660FileSystem::Open(const CString& path, int mode)
{
    ISO9660Directory* directoryEntry;
    CString fileName;

    int lastIndexOfSlash = path.lastIndexOf('/');

    if (lastIndexOfSlash == -1)
    {
        directoryEntry = this->rootDirectory;
        fileName = path;
    }
    else
    {
        CString directoryPath = path.substring(0, lastIndexOfSlash);
        fileName  = path.substring(lastIndexOfSlash + 1, path.getLength() - lastIndexOfSlash);
        directoryEntry = FindDirectoryEntry(this->rootDirectory, directoryPath);

        if (directoryEntry == NULL) return NULL;
    }

    ISO9660File* file = FindFileEntry(directoryEntry, fileName);
    if (file == NULL) return NULL;

    return file;
}

bool ISO9660FileSystem::Close(File* file)
{
    delete file;
    return false;
}

bool ISO9660FileSystem::CreateFile(const CString& path)
{
    return false;
}

bool ISO9660FileSystem::RemoveFile(const CString& path)
{
    return false;
}

bool ISO9660FileSystem::CreateDirectory(const CString& path)
{
    return false;
}

bool ISO9660FileSystem::RemoveDirectory(const CString& path)
{
    return false;
}

bool ISO9660FileSystem::IsExistDirectory(const MonAPI::CString& path)
{
    ISO9660Directory* entry = FindDirectoryEntry(this->rootDirectory, path);
    return entry != NULL;
}

_A<FileSystemEntry*> ISO9660FileSystem::GetFileSystemEntries(const CString& path)
{
    _A<FileSystemEntry*> noResult(0);

    ISO9660Directory* entry = FindDirectoryEntry(this->rootDirectory, path);
    if (entry == NULL)
    {
        return noResult;
    }

    SetDetailInformation(entry);

    dword readSize = ((dword)((entry->attribute.size + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;

    byte* buffer = new byte[readSize];

    if (buffer == NULL)
    {
        this->lastError = MEMORY_ALLOCATE_ERROR;
        return noResult;
    }

    bool readResult = this->cd->read(entry->attribute.extent, buffer, readSize) == 0;

    if (!readResult)
    {
        this->lastError = READ_ERROR;
        delete buffer;
        return noResult;
    }

    HList<FileSystemEntry*> entries;
    for (dword position = 0 ; position < readSize;)
    {
        ISODirectoryEntry* iEntry = (ISODirectoryEntry*)((dword)buffer + position);

        if (iEntry->length == 0)
        {
            // check next sector
            position = ((position + SECTOR_SIZE - 1) / SECTOR_SIZE) * SECTOR_SIZE;
            continue;
        }

        AddToFileSystemEntryList(&entries, entry, iEntry);

        position += iEntry->length;
    }
    delete[] buffer;

    _A<FileSystemEntry*> ret(entries.size());

    for (int i = 0; i < entries.size(); i++)
    {
        ret[i] = entries[i];
    }

    return ret;
}

/*----------------------------------------------------------------------
    ISO9660FileSystem private functions
----------------------------------------------------------------------*/
bool ISO9660FileSystem::ReadVolumeDescriptor()
{
    int i;
    bool primaryVolumeDescriptorFound = false;

    ISOBaseVolumeDescriptor* descriptor = new ISOBaseVolumeDescriptor;
    if (descriptor == NULL)
    {
        this->lastError = MEMORY_ALLOCATE_ERROR;
        return false;
    }
    for (i = 16; i < 100; i++)
    {
        /* read */
        bool readResult = this->cd->read(i, descriptor, SECTOR_SIZE) == 0;
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
            primaryVolumeDescriptorFound = true;
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

bool ISO9660FileSystem::SetDirectoryCache()
{
    byte* buffer;

    /* read path table */
    if ((buffer = ReadPathTableIntoBuffer()) == NULL)
    {
        return false;
    }

    /* create DirectoryEntries from path table */
    HList<ISO9660Directory*> directoryList;

    CreateDirectoryListFromPathTable(&directoryList, buffer);
    delete[] buffer;

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

    /* parent of root is root */
    directoryList[0]->parent = directoryList[0];

    return true;
}

void ISO9660FileSystem::CreateDirectoryListFromPathTable(HList<ISO9660Directory*>* list, byte* buffer)
{
    for (dword id = 1, position = 0; position < this->pdescriptor.path_table_size_l; id++)
    {
        ISO9660Directory* entry = new ISO9660Directory();

        ISOPathTableEntry* pathEntry = (ISOPathTableEntry*)(buffer + position);
        entry->attribute.id       = id;
        entry->attribute.extent   = pathEntry->extent;
        entry->attribute.parentID = pathEntry->parentDirectory;

        if (pathEntry->length == 1 && pathEntry->name[0] == 0x00)
        {
            entry->SetName(".");
        }
        else if (pathEntry->length == 1 && pathEntry->name[0] == 0x01)
        {
            entry->SetName("..");
        }
        else
        {
            entry->SetName(CString(pathEntry->name, pathEntry->length));
        }

        list->add(entry);

        /* next path table entry */
        position += pathEntry->length + sizeof(ISOPathTableEntry) + (pathEntry->length % 2 ? 1 : 0);
    }
}

byte* ISO9660FileSystem::ReadPathTableIntoBuffer()
{
    dword readSize = ((dword)((this->pdescriptor.path_table_size_l + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;
    byte* buffer = new byte[readSize];

    if (buffer == NULL)
    {
        this->lastError = MEMORY_ALLOCATE_ERROR;
        return NULL;
    }

    bool readResult = this->cd->read(pdescriptor.type_l_path_table, buffer, readSize) == 0;

    return readResult ? buffer : NULL;
}

void ISO9660FileSystem::SetDetailInformation(ISO9660Directory* to, ISODirectoryEntry* from)
{
    FileDate* createDate = to->GetCreateDate();

    to->SetName(GetProperName(CString(from->name, from->name_len)));

    to->attribute.extent= from->extent_l;
    to->attribute.size  = from->size_l;
    createDate->SetYear(from->date[0] + 1900);
    createDate->SetMonth(from->date[1]);
    createDate->SetDay(from->date[2]);
    createDate->SetHour(from->date[3]);
    createDate->SetMinute(from->date[4]);
    createDate->SetSecond(from->date[5]);
    to->hasDetail = true;
}

void ISO9660FileSystem::SetDetailInformation(ISO9660File* to, ISODirectoryEntry* from)
{
    FileDate* createDate = to->GetCreateDate();

    to->SetName(GetProperName(CString(from->name, from->name_len)));

    to->attribute.extent= from->extent_l;
    to->attribute.size  = from->size_l;
    createDate->SetYear(from->date[0] + 1900);
    createDate->SetMonth(from->date[1]);
    createDate->SetDay(from->date[2]);
    createDate->SetHour(from->date[3]);
    createDate->SetMinute(from->date[4]);
    createDate->SetSecond(from->date[5]);
    to->hasDetail = true;
}

void ISO9660FileSystem::SetDirectoryRelation(HList<ISO9660Directory*>* list, ISO9660Directory* directory)
{
    dword self = directory->attribute.id;

    for (int i = 0; i < list->size(); i++)
    {
        ISO9660Directory* entry = list->get(i);

        if (entry->attribute.parentID != self) continue;
        if (i == 0 && entry->attribute.parentID == 1) continue;

        directory->children.add(entry);
        entry->parent = directory;
    }
}

CString ISO9660FileSystem::GetProperName(const CString& name)
{
    CString result = name;

    if (result[0] == 0x00)
    {
        result = ".";
    }
    else if (result[0] == 0x01)
    {
        result = "..";
    }

    if (name.indexOf(';') != -1)
    {
        result = result.split(';')[0];
    }

    if (result.endsWith(".") && result != "." && result != "..")
    {
        result = result.substring(0, result.getLength() - 1);
    }
    return result;
}

ISO9660Directory* ISO9660FileSystem::FindDirectoryEntry(ISO9660Directory* root, const CString& path)
{
    bool found;
    _A<CString> elements = path.split('/');

    FOREACH (CString, element, elements)
    {
        if (element == ".")
        {
            continue;
        }
        else if (element == "..")
        {
            root = root->parent;
            continue;
        }
        else if (element == "" || element == NULL)
        {
            continue;
        }

        found = false;
        for (int i = 0; i < root->children.size(); i++)
        {
            if (root->children[i]->GetName() == element)
            {
                root = (ISO9660Directory*)root->children[i];
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
ISO9660File* ISO9660FileSystem::FindFileEntry(ISO9660Directory* directory, const CString& fileName)
{
    SetDetailInformation(directory);

    dword readSize = ((dword)((directory->attribute.size + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;
    byte* buffer = new byte[readSize];

    if (buffer == NULL)
    {
        this->lastError = MEMORY_ALLOCATE_ERROR;
        return NULL;
    }

    bool readResult = this->cd->read(directory->attribute.extent, buffer, readSize) == 0;

    if (!readResult)
    {
        this->lastError = READ_ERROR;
        printf("device read error\n");
        delete buffer;
        return NULL;
    }

    for (dword position = 0; position < readSize;)
    {
        ISODirectoryEntry* iEntry = (ISODirectoryEntry*)(buffer + position);

        if (iEntry->length == 0)
        {
            // check next sector
            position = ((position + SECTOR_SIZE - 1) / SECTOR_SIZE) * SECTOR_SIZE;
            continue;
        }

        CString name = GetProperName(CString(iEntry->name, iEntry->name_len));

        if (iEntry->directory == 0 && fileName == name)
        {
            ISO9660File* foundFile = new ISO9660File(this);

            SetDetailInformation(foundFile, iEntry);

            delete[] buffer;
            return foundFile;
        }

        position += iEntry->length;
        iEntry = (ISODirectoryEntry*)(buffer + position);
    }

    delete[] buffer;
    return NULL;
}

bool ISO9660FileSystem::SetDetailInformation(ISO9660Directory* entry)
{
    if (entry->hasDetail) return true;

    if (!(entry->parent->hasDetail))
    {
        SetDetailInformation(entry->parent);
    }

    dword readSize = ((dword)((entry->parent->attribute.size + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;
    byte* buffer = new byte[readSize];

    if (buffer == NULL)
    {
        this->lastError = MEMORY_ALLOCATE_ERROR;
        return false;
    }

    bool readResult = this->cd->read(entry->parent->attribute.extent, buffer, readSize) == 0;

    if (!readResult)
    {
        this->lastError = READ_ERROR;
        delete[] buffer;
        return false;
    }

    HList<FileSystemEntry*>* children = &(entry->parent->children);

    for (dword position = 0; position < readSize;)
    {
        ISODirectoryEntry* iEntry = (ISODirectoryEntry*)(buffer + position);
        CString name = CString(iEntry->name, iEntry->name_len);

        if (iEntry->length == 0) break;

        for (int i = 0; i < children->size(); i++)
        {
            ISO9660Directory* child = (ISO9660Directory*)children->get(i);
            if (name != child->GetName()) continue;

            SetDetailInformation(child, iEntry);
        }

        position += iEntry->length;
    }
    delete[] buffer;
    return true;
}

bool ISO9660FileSystem::SetDetailInformation(ISO9660File* entry)
{
    if (entry->hasDetail) return true;

    if (!(entry->parent->hasDetail))
    {
        SetDetailInformation(entry->parent);
    }

    dword readSize = ((dword)((entry->parent->attribute.size + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;
    byte* buffer = new byte[readSize];

    if (buffer == NULL)
    {
        this->lastError = MEMORY_ALLOCATE_ERROR;
        return false;
    }

    bool readResult = this->cd->read(entry->parent->attribute.extent, buffer, readSize) == 0;

    if (!readResult)
    {
        this->lastError = READ_ERROR;
        delete[] buffer;
        return false;
    }

    HList<FileSystemEntry*>* children = &(entry->parent->children);

    for (dword position = 0; position < readSize;)
    {
        ISODirectoryEntry* iEntry = (ISODirectoryEntry*)(buffer + position);
        CString name = CString(iEntry->name, iEntry->name_len);

        if (iEntry->length == 0) break;

        for (int i = 0; i < children->size(); i++)
        {
            ISO9660Directory* child = (ISO9660Directory*)children->get(i);
            if (name != child->GetName()) continue;

            SetDetailInformation(child, iEntry);
        }

        position += iEntry->length;
    }
    delete[] buffer;
    return true;
}

void ISO9660FileSystem::DeleteEntry(FileSystemEntry* entry)
{
    if (entry == NULL) return;

    if (!entry->IsDirectory())
    {
        delete entry;
        return;
    }

    ISO9660Directory* directory = (ISO9660Directory*)entry;

    for (int i = 0; i < directory->children.size(); i++)
    {
        DeleteEntry(directory->children[i]);
    }

    delete entry;
}

void ISO9660FileSystem::AddToFileSystemEntryList(HList<FileSystemEntry*>* entries, ISO9660Directory* directory, ISODirectoryEntry* iEntry)
{
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

        FileSystemEntry* cache = NULL;

        for (int i = 0; i < directory->children.size(); i++)
        {
            if (name == directory->children[i]->GetName())
            {
                cache = directory->children[i];
                break;
            }
        }

        if (cache != NULL && cache->IsDirectory())
        {
            SetDetailInformation((ISO9660Directory*)cache);
            entries->add(cache);
        }
        else if (cache != NULL && !cache->IsDirectory())
        {
            SetDetailInformation((ISO9660File*)cache);
            entries->add(cache);
        }
        else if (iEntry->directory == 1)
        {
            ISO9660Directory* dir = new ISO9660Directory();
            SetDetailInformation(dir, iEntry);
            directory->children.add(dir);
            entries->add(dir);
        }
        else
        {
            ISO9660File* file = new ISO9660File(this);
            SetDetailInformation(file, iEntry);
            directory->children.add(file);
            entries->add(file);
        }
}

int ISO9660FileSystem::lookup(vnode* diretory, const string& file, vnode** found)
{
    if (diretory->v_type != VDIR) return MONA_ERROR_INVALID_ARGUMENTS;

    vnode* v = cacher_->lookup(diretory, file);
    if (v != NULL)
    {
        *found = v;
        return MONA_OK;
    }

    ISO9660Directory* directoryEntry = (ISO9660Directory*)diretory->fnode;
    ASSERT(directoryEntry != NULL);
    ISO9660File* fileEntry = FindFileEntry(directoryEntry, file.c_str());

    if (fileEntry != NULL)
    {
        vnode* newVnode = Vnode::alloc();
        newVnode->fnode  = fileEntry;
        newVnode->v_type = VREG;
        cacher_->add(diretory, file, newVnode);
        *found = newVnode;
        return MONA_OK;
    }


    // directoryから現在のパスを得る
    // update cache/add cache
    // そのパスから file を探す
    // すでにvnodeを作っているかどうかで分岐。
    // directory cache, directory entry cache
    return MONA_OK;
}
