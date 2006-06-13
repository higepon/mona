/*!
    \file  FileSystem.h
    \brief FileSystem

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2005/01/23 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#ifndef _ISO9660_FILE_SYSTEM_
#define _ISO9660_FILE_SYSTEM_

#ifdef MONA
#include <fat_write/IStorageDevice.h>

#else
#include "IStorageDevice.h"
#endif

#include "VnodeCacher.h"
#include "FileSystem.h"
#include "ISO9660.h"
#include "ISO9660File.h"
#include "ISO9660Directory.h"

class ISO9660File;

/*----------------------------------------------------------------------
    ISO9660FileSystem
----------------------------------------------------------------------*/
class ISO9660FileSystem : public FileSystem
{
public:
    ISO9660FileSystem(IStorageDevice* cd, VnodeCacher* cacher, VnodeManager* vmanager);
    virtual ~ISO9660FileSystem();

public:
    enum
    {
        SECTOR_SIZE = 2048,
        NO_ERROR = 0,
        READ_ERROR,
        MEMORY_ALLOCATE_ERROR,
        VOLUME_DESCRIPTOR_NOT_FOUND,
        DIRECTORY_NOT_FOUND,
    };

public:
    virtual bool Initialize();
    virtual bool Read(dword lba, byte* buffer, dword size);

public:
    virtual File* Open(const MonAPI::CString& path, int mode);
    virtual bool Close(File* file);
    virtual bool CreateFile(const MonAPI::CString& path);
    virtual bool RemoveFile(const MonAPI::CString& path);
    virtual bool CreateDirectory(const MonAPI::CString& path);
    virtual bool RemoveDirectory(const MonAPI::CString& path);
    virtual bool IsExistDirectory(const MonAPI::CString& path);
    virtual _A<FileSystemEntry*> GetFileSystemEntries(const MonAPI::CString& path);
    virtual int GetLastError() {return this->lastError;}
    virtual int lookup(Vnode* diretory, const std::string& file, Vnode** found, int type);
    virtual int open(Vnode* file, int mode);
    virtual int read(Vnode* file, io::Context* context);
    //    virtual int readdir(Vnode* directory, std::vector<FileSystemEntr*>* entries);
    virtual int seek(Vnode* file, dword offset, dword origin);
    virtual int close(Vnode* file);
    virtual Vnode* getRootDirectory() const;

private:
    enum
    {
        ISO_PRIMARY_VOLUME_DESCRIPTOR = 1,
        ISO_END_VOLUME_DESCRIPTOR     = 255
    };

private:
    bool ReadVolumeDescriptor();
    bool SetDirectoryCache();
    void CreateDirectoryListFromPathTable(HList<ISO9660Directory*>* list, byte* buffe);
    byte* ReadPathTableIntoBuffer();
    void SetDetailInformation(ISO9660Directory* to, ISODirectoryEntry* from);
    void SetDirectoryRelation(HList<ISO9660Directory*>* list, ISO9660Directory* directory);
    MonAPI::CString GetProperName(const MonAPI::CString& name);
    ISO9660Directory* FindDirectoryEntry(ISO9660Directory* root, const MonAPI::CString& path);
    ISO9660File* FindFileEntry(ISO9660Directory* directory, const MonAPI::CString& fileName);
    bool SetDetailInformation(ISO9660Directory* entry);
    bool SetDetailInformation(ISO9660File* entry);
    void SetDetailInformation(ISO9660File* to, ISODirectoryEntry* from);
    void DeleteEntry(FileSystemEntry* entry);
    void AddToFileSystemEntryList(HList<FileSystemEntry*>* entries, ISO9660Directory* directory, ISODirectoryEntry* iEntry);

private:
    int lastError;
    IStorageDevice* cd;
    ISOPrimaryVolumeDescriptor pdescriptor;
    ISO9660Directory* rootDirectory;

protected:
    Vnode* root_;
    VnodeCacher* cacher_;
    VnodeManager* vmanager_;
};

#endif
