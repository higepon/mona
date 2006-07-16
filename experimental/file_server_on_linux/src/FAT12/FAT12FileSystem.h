#ifndef __FAT12FILESYSTEM_H__
#define __FAT12FILESYSTEM_H__

#include "FileSystem.h"
#include "IStorageDevice.h"
#include "VnodeManager.h"
#include <vector>

class FAT12FileSystem : public FileSystem
{
public:
    FAT12FileSystem(IStorageDevice* drive, VnodeManager* vmanager);
    virtual ~FAT12FileSystem();

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

protected:
    IStorageDevice* drive_;
    VnodeManager* vmanager_;
    Vnode* root_;
};

#endif // __FAT12FILESYSTEM_H__
