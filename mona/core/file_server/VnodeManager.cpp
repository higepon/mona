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
    int ret = MONA_FAILURE;
    Vnode* root = directory;
    for (uint32_t i = 0; i < directories.size(); i++)
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
    if (filename == "/") {
        dir = root_;
    }
    else
    {
        if (lookup(root_, filename, &dir, Vnode::DIRECTORY) != MONA_SUCCESS)
        {
            return MONA_ERROR_ENTRY_NOT_FOUND;
        }
    }
    if (dir->fs->readdir(dir, mem) != MONA_SUCCESS) return MONA_FAILURE;
    return MONA_SUCCESS;
}

int VnodeManager::open(const std::string& name, int mode, bool create, uint32_t tid, uint32_t* fileID)
{
    // now fullpath only. fix me
    if (name.compare(0, 1, "/") != 0) return MONA_ERROR_INVALID_ARGUMENTS;

    if (create)
    {
        Vnode* targetDirectory = NULL;
        uint32_t foundIndex = name.find_last_of('/');
        string filename = name;
        if (foundIndex == name.npos)
        {
            targetDirectory = root_;
        }
        else
        {
            string dirPath = name.substr(1, foundIndex - 1);
            if (lookup(root_, dirPath, &targetDirectory, Vnode::DIRECTORY) != MONA_SUCCESS)
            {
                return MONA_ERROR_ENTRY_NOT_FOUND;
            }
            filename = name.substr(foundIndex + 1, name.size() - foundIndex);
        }
        int ret = targetDirectory->fs->create(targetDirectory, filename);
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
    fileInfoMap_.insert(pair< uint32_t, FileInfo* >(*fileID, fileInfo));
    return MONA_SUCCESS;
}

Vnode* VnodeManager::alloc()
{
    Vnode* v = new Vnode;
    v->mountedVnode = NULL;
    return v;
}

int VnodeManager::read(uint32_t fileID, uint32_t size, monapi_cmemoryinfo** mem)
{
    FileInfoMap::iterator it = fileInfoMap_.find(fileID);
    if (it == fileInfoMap_.end())
    {
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }
    io::FileInfo* fileInfo = (*it).second;
    io::Context* context = &(fileInfo->context);
    context->size = size;
    int result = fileInfo->vnode->fs->read(fileInfo->vnode, context);
    *mem = context->memory;
    return result;
}

int VnodeManager::write(uint32_t fileID, uint32_t size, monapi_cmemoryinfo* mem)
{
    FileInfoMap::iterator it = fileInfoMap_.find(fileID);
    if (it == fileInfoMap_.end())
    {
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }
    io::FileInfo* fileInfo = (*it).second;
    io::Context* context = &(fileInfo->context);
    context->size = size;
    context->memory = mem;
    return fileInfo->vnode->fs->write(fileInfo->vnode, context);
}

int VnodeManager::close(uint32_t fileID)
{
    FileInfoMap::iterator it = fileInfoMap_.find(fileID);
    if (it == fileInfoMap_.end())
    {
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }

    FileInfo* fileInfo = (*it).second;
    fileInfo->context.memory = NULL;
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

int VnodeManager::stat(uint32_t fileID, Stat* st)
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

int VnodeManager::seek(uint32_t fileID, uint32_t offset, uint32_t origin)
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
    uint32_t index = 0;
    uint32_t next = 0;
    while ((index = str.find_first_of(ch, next)) != string::npos)
    {
        v.push_back(string(str.begin() + next, str.begin() + index));
        next = index + 1;
    }
    v.push_back(string(str.begin() + next, str.end()));
}
