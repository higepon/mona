#ifndef __VNODEMANAGER_H__
#define __VNODEMANAGER_H__

#include <vector>
#include "vnode.h"
#include "VnodeCacher.h"

class VnodeManager
{
public:
    VnodeManager();
    virtual ~VnodeManager();

public:
    void setRoot(Vnode* root) {root_ = root;}
    int mount(Vnode* a, const std::string& path, Vnode* b);
    int lookup(Vnode* diretory, const std::string& file, Vnode** found, int type = Vnode::REGULAR);
    int lookupOne(Vnode* directory, const std::string& file, Vnode** found, int type);
    int open(const std::string& name, intptr_t mode, uint32_t tid, uint32_t* fileID);
    int read(uint32_t fileID, uint32_t size, MonAPI::SharedMemory** mem);
    int write(uint32_t fileID, uint32_t size, MonAPI::SharedMemory* mem);
    int seek(uint32_t fileID, int32_t offset, uint32_t origin);
    int stat(uint32_t fileID, Stat* st);
    int stat(const std::string& path, Stat* st);
    int close(uint32_t fileID);
    int create(const std::string& name);
    int create_directory(const std::string& name);
    int delete_file(const std::string& name);
    int delete_directory(const std::string& name);
    int read_directory(const std::string& name, MonAPI::SharedMemory** mem);
    // N.B fileID should be posive, since negative values indicates error.
    intptr_t fileID(Vnode* file , uint32_t tid) {return abs((intptr_t)file | tid);} // temporary
    Vnode* alloc();
    VnodeCacher* cacher() {return cacher_;}
    void split(std::string str, char ch, std::vector<std::string>& v);
private:
    int delete_file(const std::string& name, bool isDirectory);
    int create_file(const std::string& name, bool isDirectory);

protected:

private:
    typedef std::map<uint32_t, io::FileInfo*> FileInfoMap;
    Vnode* root_;
    FileInfoMap fileInfoMap_;
    VnodeCacher* cacher_;
};

#endif // __VNODEMANAGER_H__
