#ifndef __ISO9660FILESYSTEM_H__
#define __ISO9660FILESYSTEM_H__

#include "FileSystem.h"
#include "IStorageDevice.h"
#include "VnodeManager.h"
#include "ISO9660.h"
#include <vector>

class ISO9660FileSystem : public FileSystem
{
public:
    ISO9660FileSystem(IStorageDevice* drive, VnodeManager* vmanager);
    virtual ~ISO9660FileSystem();

public:
    virtual int initialize();
    virtual int lookup(Vnode* diretory, const std::string& file, Vnode** found, int type);
    virtual int open(Vnode* file, int mode);
    virtual int read(Vnode* file, struct io::Context* context);
    virtual int seek(Vnode* file, dword offset, dword origin);
    virtual int readdir(Vnode* directory, monapi_cmemoryinfo** entries);
    virtual int close(Vnode* file);
    virtual int stat(Vnode* file, Stat* st);
    virtual Vnode* getRoot() const;
    virtual void destroyVnode(Vnode* vnode);

    enum
    {
        SECTOR_SIZE = 2048,
    };


private:
    int readVolumeDescriptor();
    byte* readPathTableIntoBuffer();
    int setDirectoryCache();
    void createDirectoryListFromPathTable(iso9660::EntryList* list, byte* buffer);
    void setDetailInformation(iso9660::Entry* to, iso9660::DirectoryEntry* from);
    bool setDetailInformation(iso9660::Entry* entry);
    std::string getProperName(const std::string& name);
    void setDirectoryRelation(iso9660::EntryList* list, iso9660::Entry* directory);
    void deleteEntry(iso9660::Entry* entry);
    void split(std::string str, char ch, std::vector<std::string>& v);
    iso9660::Entry* lookupDirectory(iso9660::Entry* root, const std::string& path);
    iso9660::Entry* lookupFile(iso9660::Entry* directory, const std::string& fileName);
    iso9660::Entry* setupEntry(iso9660::DirectoryEntry* from);
    byte* readdirToBuffer(iso9660::Entry* directory, dword readSize);
    enum
    {
        ISO_PRIMARY_VOLUME_DESCRIPTOR = 1,
        ISO_END_VOLUME_DESCRIPTOR     = 255
    };

protected:
    IStorageDevice* drive_;
    VnodeManager* vmanager_;
    iso9660::PrimaryVolumeDescriptor pdescriptor_;
    iso9660::Entry* rootDirectory_;
    Vnode* root_;
};

#endif // __ISO9660FILESYSTEM_H__
