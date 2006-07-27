#include "VnodeManager.h"

using namespace std;
using namespace io;

VnodeManager::VnodeManager()
{
    cacher_ = new VnodeCacher;
}

VnodeManager::~VnodeManager()
{
    delete cacher_;
}

int VnodeManager::lookup(Vnode* directory, const string& file, Vnode** found, int type)
{
    vector<string> directories;
    split(file, '/', directories);
    int ret;
    Vnode* root = directory;
    for (dword i = 0; i < directories.size(); i++)
    {
        string name = directories[i];
        int vtype = (i == directories.size() - 1) ? type : Vnode::DIRECTORY;
        ret = root->fs->lookup(root, name, found, vtype);
        if (ret != MONA_SUCCESS) return ret;

        // link
        if ((*found)->mountedVnode != NULL)
        {
            *found = (*found)->mountedVnode;
        }
        if (i != directories.size() -1)
        {
            root = *found;
        }
    }
    return ret;
}

int VnodeManager::readdir(const std::string&name, monapi_cmemoryinfo** mem)
{
    // now fullpath only. fix me
    if (name.compare(0, 1, "/") != 0) return MONA_ERROR_INVALID_ARGUMENTS;

    // remove first '/'. fix me
    string filename;
    if (name.size() == 1)
    {
        filename = name;
    }
    else
    {
        filename = name.substr(1, name.size() - 1);
    }

    Vnode* dir;
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    if (filename == "/") {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        dir = root_;
    }
    else
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        if (lookup(root_, filename, &dir, Vnode::DIRECTORY) != MONA_SUCCESS)
        {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            return MONA_ERROR_ENTRY_NOT_FOUND;
        }
    }
    printf("%s %s:%d dir=%x\n", __func__, __FILE__, __LINE__, dir);fflush(stdout);// debug
    if (dir->fs->readdir(dir, mem) != MONA_SUCCESS) return MONA_FAILURE;
    return MONA_SUCCESS;
}

int VnodeManager::open(const std::string& name, int mode, bool create, dword tid, dword* fileID)
{
    // now fullpath only. fix me
    if (name.compare(0, 1, "/") != 0) return MONA_ERROR_INVALID_ARGUMENTS;

    if (create)
    {
        Vnode* targetDirectory = NULL;
        int foundIndex = name.find_last_of('/');
        if (foundIndex == name.npos)
        {
            targetDirectory = root_;
        }
        else
        {
            string dirPath = name.substr(1, foundIndex);
            if (lookup(root_, dirPath, &targetDirectory, Vnode::DIRECTORY) != MONA_SUCCESS)
            {
                return MONA_ERROR_ENTRY_NOT_FOUND;
            }
        }
        int ret = targetDirectory->fs->create(targetDirectory, name);
        if (MONA_SUCCESS != ret)
        {
            return ret;
        }
    }

    // remove first '/'. fix me
    string filename = name.substr(1, name.size() - 1);

    Vnode* file;

    if (lookup(root_, filename, &file) != MONA_SUCCESS)
    {
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }

    int ret = file->fs->open(file, mode);
    if (MONA_SUCCESS != ret)
    {
        return ret;
    }
    *fileID = this->fileID(file, tid);
//     if (fileInfoMap_.find(*fileID) != fileInfoMap_.end())
//     {
//         printf("error fix me!!! %s %s:%d\n", __func__, __FILE__, __LINE__);
//         exit(-1);
//     }
    FileInfo* fileInfo = new FileInfo;
    fileInfo->vnode = file;
    fileInfo->context.tid = tid;
    fileInfoMap_.insert(pair< dword, FileInfo* >(*fileID, fileInfo));
    return MONA_SUCCESS;
}

Vnode* VnodeManager::alloc()
{
    Vnode* v = new Vnode;
    assert(v != NULL);
    v->mountedVnode = NULL;
    return v;
}

int VnodeManager::read(dword fileID, dword size, monapi_cmemoryinfo** mem)
{
    FileInfoMap::iterator it = fileInfoMap_.find(fileID);
    if (it == fileInfoMap_.end())
    {
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }
    io::FileInfo* fileInfo = (*it).second;
    io::Context* context = &(fileInfo->context);
    context->size = size;
    context->memory = monapi_cmemoryinfo_new();
    if (!monapi_cmemoryinfo_create(context->memory, size, MONAPI_FALSE))
    {
        monapi_cmemoryinfo_delete(context->memory);
        return MONA_ERROR_MEMORY_NOT_ENOUGH;
    }
    *mem = context->memory;
    return fileInfo->vnode->fs->read(fileInfo->vnode, context);
}

int VnodeManager::close(dword fileID)
{
    FileInfoMap::iterator it = fileInfoMap_.find(fileID);
    if (it == fileInfoMap_.end())
    {
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }

    FileInfo* fileInfo = (*it).second;
    Vnode* file = fileInfo->vnode;
    int ret = file->fs->close(file);
    if (MONA_SUCCESS != ret)
    {
        return ret;
    }
    fileInfoMap_.erase(fileID);
    delete fileInfo;
    return MONA_SUCCESS;
}

int VnodeManager::stat(dword fileID, Stat* st)
{
    FileInfoMap::iterator it = fileInfoMap_.find(fileID);
    if (it == fileInfoMap_.end())
    {
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }

    FileInfo* fileInfo = (*it).second;
    Vnode* file = fileInfo->vnode;
    return file->fs->stat(file, st);
}

int VnodeManager::seek(dword fileID, dword offset, dword origin)
{
    FileInfoMap::iterator it = fileInfoMap_.find(fileID);
    if (it == fileInfoMap_.end())
    {
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }

    FileInfo* fileInfo = (*it).second;
    Vnode* file = fileInfo->vnode;
    int ret = file->fs->seek(file, offset, origin);
    if (MONA_SUCCESS != ret)
    {
        return ret;
    }
    fileInfo->context.offset = offset;
    fileInfo->context.origin = origin;
    return MONA_SUCCESS;
}

int VnodeManager::mount(Vnode* a, const std::string& path, Vnode* b)
{
    cacher_->add(a, path, b);
    return MONA_SUCCESS;
}

void VnodeManager::split(string str, char ch, vector<string>& v)
{
    dword index = 0;
    dword next = 0;
    while ((index = str.find_first_of(ch, next)) != string::npos)
    {
        v.push_back(string(str.begin() + next, str.begin() + index));
        next = index + 1;
    }
    v.push_back(string(str.begin() + next, str.end()));
}
