#ifdef ON_LINUX
#include "servers/file.h"
#include "servers/servers.h"
#include "BinaryTree.h"
#else
#include <monapi.h>
#include <sys/BinaryTree.h>
#include <monapi/io.h>
#endif
#include <monapi/Assert.h>
#include <monapi/messages.h>
#include "FileServer.h"
#ifdef USE_BZIP2
#include "bzip2.h"
#endif
#include "Message.h"
#include "dtk5s.h"
#include "IDEDriver.h"
#include "vnode.h"
#include <map>

using namespace MonAPI;
using namespace std;

typedef struct
{
    Vnode* vnode;
    io::Context context;
} FileInfo;

typedef map< dword, FileInfo* > FileInfoMap;

VnodeManager* vmanager;
FileInfoMap fileInfoMap;

dword fileID(Vnode* file , dword tid)
{
    return (dword)file | tid; // temporary
}

void MessageLoop()
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg)) continue;

        switch (msg.header)
        {
            case MSG_VFS_FILE_OPEN:
            {
                ChangeDrive(DRIVE_CD0);
                Vnode* file;
                if (MONA_OK == vmanager->open(msg.str, 0, false, &file))
                {
                    dword tid = msg.from; // temporary
                    dword id = fileID(file, tid);
                    if (fileInfoMap.find(id) != fileInfoMap.end())
                    {
                        printf("error fix me!!! %s %s:%d\n", __func__, __FILE__, __LINE__);
                        exit(-1);
                    }
                    FileInfo* fileInfo = new FileInfo;
                    fileInfo->vnode = file;
                    printf("open vnode = %x %s %s:%d\n", (dword)file, __func__, __FILE__, __LINE__);fflush(stdout);
                    fileInfo->context.tid = tid;
                    fileInfoMap.insert(pair< dword, FileInfo* >(id, fileInfo));
                    printf("%s %s:%d id = %d\n", __func__, __FILE__, __LINE__, id);fflush(stdout);
                    Message::reply(&msg, id);
                }
                else
                {
                    printf("file open error\n");
                    Message::reply(&msg, 0);
                }
                break;
            }
            case MSG_VFS_FILE_READ:
            {
                dword id = msg.arg1;
                FileInfoMap::iterator it = fileInfoMap.find(id);
                if (it == fileInfoMap.end())
                {
                    Message::reply(&msg, 0);
                }

                FileInfo* fileInfo = (*it).second;
                fileInfo->context.size = msg.arg2;

                monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
                if (!monapi_cmemoryinfo_create(ret, msg.arg2, MONAPI_FALSE))
                {
                    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
                    monapi_cmemoryinfo_delete(ret);
                    Message::reply(&msg, 0);
                }

                io::Buffer* buffer = new io::Buffer;
                buffer->pointer = ret->Data;
                buffer->size = ret->Size;
                fileInfo->context.buffer = buffer;
                vmanager->read(fileInfo->vnode, &(fileInfo->context));
                Message::reply(&msg, ret->Handle, ret->Size);
                break;
            }

            case MSG_FILE_READ_DATA:
            {
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
                monapi_cmemoryinfo* mi = ReadFile(msg.str, msg.arg1);
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
                if (mi != NULL)
                {
                    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
                    Message::reply(&msg, mi->Handle, mi->Size);
                    monapi_cmemoryinfo_delete(mi);
                }
                else
                {
                    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
                    Message::reply(&msg);
                }
                break;
            }
            case MSG_FILE_READ_DIRECTORY:
            {
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
                monapi_cmemoryinfo* mi = ReadDirectory(msg.str, msg.arg1);
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);

    monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
    printf("%s %s:%d size=%d\n", __func__, __FILE__, __LINE__, mi->Size);fflush(stdout);
    int size = *(int*)mi->Data;
    for (int i = 0; i < size; i++, p++)
    {
        printf("%s\n", p->name);
    }


                if (mi != NULL)
                {
                    Message::reply(&msg, mi->Handle, mi->Size);
                    monapi_cmemoryinfo_delete(mi);
                }
                else
                {
                    Message::reply(&msg);
                }
                break;
            }
            case MSG_FILE_OPEN:
            {
                dword result = Open(msg.str);
                Message::reply(&msg, result);
                break;
            }
            case MSG_FILE_GET_SIZE:
            {
                dword result = GetFileSize(msg.arg1);
                Message::reply(&msg, result);
                break;
            }
            case MSG_FILE_CLOSE:
            {
                bool result = Close(msg.arg1);
                Message::reply(&msg, result ? MONAPI_TRUE : MONAPI_FALSE);
                break;
            }
            case MSG_FILE_SEEK:
            {
                bool result = Seek(msg.arg1, msg.arg2, msg.arg3);
                Message::reply(&msg, result ? MONAPI_TRUE : MONAPI_FALSE);
                break;
            }
            case MSG_FILE_READ:
            {
                monapi_cmemoryinfo* mi = Read(msg.arg1, msg.arg2);
                if (mi != NULL)
                {
                    Message::reply(&msg, mi->Handle, mi->Size);
                    monapi_cmemoryinfo_delete(mi);
                }
                else
                {
                    Message::reply(&msg);
                }
                break;
            }
            case MSG_FILE_CHANGE_DRIVE:
            {
                int result = ChangeDrive(msg.arg1);

                Message::reply(&msg, result);
                break;
            }
            case MSG_FILE_GET_CURRENT_DRIVE:
            {
                int result = GetCurrentDrive();
                Message::reply(&msg, result);
                break;
            }
            case MSG_FILE_GET_CURRENT_DIRECTORY:
            {
                Message::reply(&msg, MONA_SUCCESS, 0, (const char*)GetCurrentDirectory());
                break;
            }
            case MSG_FILE_CHANGE_DIRECTORY:
            {
                int result = ChangeDirectory(msg.str);
                Message::reply(&msg, result);
                break;
            }
            case MSG_DISPOSE_HANDLE:
                #ifndef ON_LINUX
                MemoryMap::unmap(msg.arg1);
                #endif
                Message::reply(&msg);
                break;
            case MSG_FILE_DECOMPRESS_BZ2:
            {
#ifdef USE_BZIP2
                monapi_cmemoryinfo* mi1 = monapi_cmemoryinfo_new();

                mi1->Handle = msg.arg1;
                mi1->Size   = msg.arg2;
                monapi_cmemoryinfo* mi2 = NULL;

                if (monapi_cmemoryinfo_map(mi1))
                {
                    mi2 = BZ2Decompress(mi1);
                    monapi_cmemoryinfo_dispose(mi1);
                }

                if (mi2 != NULL)
                {
                    Message::reply(&msg, mi2->Handle, mi2->Size);
                    monapi_cmemoryinfo_delete(mi2);
                }
                else
                {
                    Message::reply(&msg);
                }
                monapi_cmemoryinfo_delete(mi1);
#else
                Message::reply(&msg);
#endif
                break;
            }
            case MSG_FILE_DECOMPRESS_BZ2_FILE:
            {
#ifdef USE_BZIP2
                monapi_cmemoryinfo* mi = BZ2DecompressFile(msg.str, msg.arg1 != 0);

                if (mi != NULL)
                {
                    Message::reply(&msg, mi->Handle, mi->Size);
                    delete mi;
                }
                else
                {
                    Message::reply(&msg);
                }
#else
                Message::reply(&msg);
#endif
                break;
            }
            case MSG_FILE_DECOMPRESS_ST5:
            {
                monapi_cmemoryinfo* mi1 = monapi_cmemoryinfo_new();
                mi1->Handle = msg.arg1;
                mi1->Size   = msg.arg2;
                monapi_cmemoryinfo* mi2 = NULL;
                if (monapi_cmemoryinfo_map(mi1))
                {
                    mi2 = ST5Decompress(mi1);
                    monapi_cmemoryinfo_dispose(mi1);
                }
                if (mi2 != NULL)
                {
                    Message::reply(&msg, mi2->Handle, mi2->Size);
                    monapi_cmemoryinfo_delete(mi2);
                }
                else
                {
                    Message::reply(&msg);
                }
                monapi_cmemoryinfo_delete(mi1);
                break;
            }
            case MSG_FILE_DECOMPRESS_ST5_FILE:
            {
                monapi_cmemoryinfo* mi = ST5DecompressFile(msg.str, msg.arg1 != 0);
                if (mi != NULL)
                {
                    Message::reply(&msg, mi->Handle, mi->Size);
                    delete mi;
                }
                else
                {
                    Message::reply(&msg);
                }
                break;
            }
        }
    }
}

int main()
{
#ifdef ON_LINUX
    Message::initialize();
#else
    if (Message::send(Message::lookupMainThread("INIT"), MSG_SERVER_START_OK) != 0)
    {
        printf("%s: INIT error\n", SVR);
        exit(1);
    }
#endif
    Initialize();

    MessageLoop();

    return 0;
}
