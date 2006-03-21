#include <monapi.h>
#include <monapi/messages.h>
#include <sys/BinaryTree.h>
#include <monapi/Assert.h>
#include <monapi/io.h>
#include "FileServer.h"
#ifdef USE_BZIP2
#include "bzip2.h"
#endif
#include "dtk5s.h"
#include "IDEDriver.h"

using namespace MonAPI;

void MessageLoop()
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg)) continue;

        switch (msg.header)
        {
            case MSG_FILE_READ_DATA:
            {
                monapi_cmemoryinfo* mi = ReadFile(msg.str, msg.arg1);
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
            case MSG_FILE_READ_DIRECTORY:
            {
                monapi_cmemoryinfo* mi = ReadDirectory(msg.str, msg.arg1);
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
                MemoryMap::unmap(msg.arg1);
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

int MonaMain(List<char*>* pekoe)
{
    if (Message::send(Message::lookupMainThread("INIT"), MSG_SERVER_START_OK) != 0)
    {
        printf("%s: INIT error\n", SVR);
        exit(1);
    }

    Initialize();

    MessageLoop();

    return 0;
}
