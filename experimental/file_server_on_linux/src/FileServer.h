#ifndef __FILESERVER_H__
#define __FILESERVER_H__

#include "servers/file.h"
#include "servers/servers.h"
#include "vnode.h"
#include "VnodeManager.h"
#include "IDEDriver.h"
#include "FAT12/FDCDriver.h"
#include "FAT12/FAT12FileSystem.h"
#include "ISO9660/ISO9660FileSystem.h"
#include "Process/ProcessFileSystem.h"
#include "dtk5s.h"
#include "Message.h"
#include <algorithm>

class FileServer
{
public:
    FileServer();
    virtual ~FileServer();

    void messageLoop();
    int initializeFileSystems();
    int initializeRootFileSystem();
    int initializeMountedFileSystems();
    monapi_cmemoryinfo* readFileAll(const std::string& file);
    monapi_cmemoryinfo* ST5DecompressFile(const char* file);
    int64_t GetST5DecompressedSize(monapi_cmemoryinfo* mi);
    monapi_cmemoryinfo* ST5Decompress(monapi_cmemoryinfo* mi);

protected:
    typedef std::vector<FileSystem*> FileSystems;
    VnodeManager* vmanager_;
    IDEDriver* cd_;
    FDCDriver* fd_;
    FileSystem* rootFS_;
    FileSystems mountedFSs_;
};

#endif // __FILESERVER_H__
