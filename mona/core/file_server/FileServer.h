#ifndef __FILESERVER_H__
#define __FILESERVER_H__

#include <sys/error.h>
#include "monalibc.h"
#include "servers/file.h"
#include "servers/servers.h"
#include "vnode.h"
#include "VnodeManager.h"
#include "IDEDriver.h"
#include "FAT12/FDCDriver.h"
#include "FAT12/FAT12FileSystem.h"
#include "ISO9660/ISO9660FileSystem.h"
//#include "Process/ProcessFileSystem.h"
#include "dtk5s.h"
#include "Message.h"
#include <algorithm>
#include "BlockDeviceDriver.h"
#define SVR "File Server"

std::string upperCase(const std::string& s);

class FileServer
{
public:
    FileServer();
    virtual ~FileServer();

    void messageLoop();
    int initializeFileSystems();
    int initializeRootFileSystem();
    int initializeMountedFileSystems();
    MonAPI::SharedMemory* readFileAll(const std::string& file, intptr_t& lastError);
    MonAPI::SharedMemory* ST5DecompressFile(const char* file);
    int64_t GetST5DecompressedSize(SharedMemory* mi);
    MonAPI::SharedMemory* ST5Decompress(SharedMemory* mi);

private:
    void send_and_release_shm(SharedMemory* mi, MessageInfo* msg);

protected:
    typedef std::vector<FileSystem*> FileSystems;
    VnodeManager vmanager_;
    IDEDriver* cd_;
    BlockDeviceDriver* bd_;
    BlockDeviceDriver* bd4_;
    FDCDriver* fd_;
    FileSystem* rootFS_;
    FileSystems mountedFSs_;
};

#endif // __FILESERVER_H__
