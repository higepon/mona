#ifndef __FILESERVER_H__
#define __FILESERVER_H__

#include "servers/file.h"
#include "servers/servers.h"
#include "vnode.h"
#include "VnodeManager.h"
#include "IDEDriver.h"
#include "ISO9660/ISO9660FileSystem.h"
#include "Process/ProcessFileSystem.h"
#include "Message.h"

class FileServer
{
public:
    FileServer();
    virtual ~FileServer();

    void messageLoop();
    int initializeFileSystems();
    int initializeRootFileSystem();
    int initializeMountedFileSystems();

protected:
    VnodeManager* vmanager_;
    IDEDriver* cd_;
    FileSystem* rootFS_;
    FileSystem* mountedFS_;
};

#endif // __FILESERVER_H__
