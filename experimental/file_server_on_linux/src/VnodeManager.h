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
    int open(const std::string& name, int mode, bool create, dword tid, dword* fileID);
    int read(dword fileID, dword size, monapi_cmemoryinfo** mem);
    int seek(dword fileID, dword offset, dword origin);
    int stat(dword fileID, Stat* st);
    int close(dword fileID);
    int readdir(const std::string& name, monapi_cmemoryinfo** mem);
    dword fileID(Vnode* file , dword tid) {return (dword)file | tid;} // temporary
    Vnode* alloc();
    VnodeCacher* cacher() {return cacher_;}
    void split(std::string str, char ch, std::vector<std::string>& v);

protected:

private:
    typedef std::map<dword, io::FileInfo*> FileInfoMap;
    Vnode* root_;
    FileInfoMap fileInfoMap_;
    VnodeCacher* cacher_;
};

#endif // __VNODEMANAGER_H__
