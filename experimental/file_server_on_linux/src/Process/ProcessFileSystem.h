#ifndef __PROCESS_FILESYSTEM_H__
#define __PROCESS_FILESYSTEM_H__

#include "FileSystem.h"
#include "VnodeManager.h"
#include <vector>
#include <string>

class Pnode
{
public:
    Pnode(dword tid, std::string name) : tid(tid), name(name) {}
    virtual ~Pnode() {}

    enum
    {
        ROOT_DIRECTORY = 0xffffffff,
    };

public:
    dword tid;
    std::string name;
};

class ProcessFileSystem : public FileSystem
{
public:
    ProcessFileSystem(VnodeManager* vmanager);
    virtual ~ProcessFileSystem();

public:
    virtual int initialize();
    virtual int lookup(Vnode* diretory, const std::string& file, Vnode** found, int type);
    virtual int open(Vnode* file, int mode);
    virtual int create(Vnode* file, const std::string& file);
    virtual int read(Vnode* file, struct io::Context* context);
    virtual int write(Vnode* file, struct io::Context* context);
    virtual int seek(Vnode* file, dword offset, dword origin);
    virtual int readdir(Vnode* directory, monapi_cmemoryinfo** entries);
    virtual int close(Vnode* file);
    virtual int stat(Vnode* file, Stat* st);
    virtual Vnode* getRoot() const;
    virtual void destroyVnode(Vnode* vnode);

protected:
    typedef std::vector<Pnode*> Pnodes;
    Pnodes pnodes_;
    VnodeManager* vmanager_;
    Vnode* root_;
    Pnode* rootPnode_;
};

#endif // __PROCESS_FILESYSTEM_H__
