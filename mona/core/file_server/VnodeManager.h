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
    int open(const std::string& name, int mode, bool create, uint32_t tid, uint32_t* fileID);
    int read(uint32_t fileID, uint32_t size, monapi_cmemoryinfo** mem);
    int write(uint32_t fileID, uint32_t size, monapi_cmemoryinfo* mem);
    int delete_file(const std::string& name);
    int seek(uint32_t fileID, uint32_t offset, uint32_t origin);
    int stat(uint32_t fileID, Stat* st);
    int close(uint32_t fileID);
    int delete_file(const std::string& name);
    int readdir(const std::string& name, monapi_cmemoryinfo** mem);
    uint32_t fileID(Vnode* file , uint32_t tid) {return (uint32_t)file | tid;} // temporary
    Vnode* alloc();
    VnodeCacher* cacher() {return cacher_;}
    void split(std::string str, char ch, std::vector<std::string>& v);

protected:

private:
    typedef std::map<uint32_t, io::FileInfo*> FileInfoMap;
    Vnode* root_;
    FileInfoMap fileInfoMap_;
    VnodeCacher* cacher_;
};

#endif // __VNODEMANAGER_H__
