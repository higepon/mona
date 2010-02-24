#ifndef __FAT12FILESYSTEM_H__
#define __FAT12FILESYSTEM_H__

#include <monalibc.h>
#include "FileSystem.h"
#include "VnodeManager.h"
#include "fat_write/fat.h"
#include "FDCDriver.h"
#include <vector>

class FAT12FileSystem : public FileSystem
{
public:
    FAT12FileSystem(FDCDriver* drive, VnodeManager* vmanager);
    virtual ~FAT12FileSystem();

public:
    virtual int initialize();
    virtual int lookup(Vnode* diretory, const std::string& file, Vnode** found, int type);
    virtual int open(Vnode* file, int mode);
    virtual int create(Vnode* dir, const std::string& file);
    virtual int read(Vnode* file, struct io::Context* context);
    virtual int write(Vnode* file, struct io::Context* context);
    virtual int seek(Vnode* file, uint32_t offset, uint32_t origin);
    virtual int readdir(Vnode* directory, monapi_cmemoryinfo** entries);
    virtual int close(Vnode* file);
    virtual int stat(Vnode* file, Stat* st);
    virtual Vnode* getRoot() const;
    virtual void destroyVnode(Vnode* vnode);

    enum
    {
        SECTOR_SIZE = 2048,
    };

    typedef struct LsInfo
    {
        int entry;
        FatFS::Directory* p;
    };

protected:
    virtual int deviceOn();
    virtual int deviceOff();
    virtual int readdirInternal(char* name, int* size, int* attribute);
    FatFS::Directory* searchFile(char* path, int* entry, int* cursor);
    FatFS::Directory* trackingDirectory(char *path, int *cursor);
    void freeDirectory(FatFS::Directory *p);

    FatFS::Directory* fatroot_;
    FatFS::FatStorage* fat_;
    FatFS::Directory* current_;
    IStorageDevice* drive_;
    FDCDriver* fd_;
    VnodeManager* vmanager_;
    Vnode* root_;
    LsInfo lsinfo_;
};

#endif // __FAT12FILESYSTEM_H__
