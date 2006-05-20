#include <stdio.h>
#include "types.h"
#include "Message.h"
#include "servers/file.h"
#include "monapi/messages.h"

using namespace MonAPI;

monapi_cmemoryinfo* monapi_call_file_read_directory(const char* path, MONAPI_BOOL prompt)
{
    monapi_cmemoryinfo* ret;
    dword tid = Message::lookupMainThread("file_server");
    MessageInfo msg;
    if (Message::sendReceive(&msg, tid, MSG_FILE_READ_DIRECTORY, prompt, 0, 0, path) != 0)
    {
        return NULL;
    }
    if (msg.arg2 == 0) return NULL;

    ret = monapi_cmemoryinfo_new();
    ret->Handle = msg.arg2;
    ret->Owner  = tid;
    ret->Size   = msg.arg3;
    monapi_cmemoryinfo_map(ret);
    return ret;
}


int main()
{
    Message::initialize();
//     dword id = Message::lookupMainThread("file_server");
//     if (id == THREAD_UNKNOWN)
//     {
//         fprintf(stderr, "file_server not found\n");
//         return -1;
//     }
//     MessageInfo msg;
//     if (Message::sendReceive(&msg, id, MSG_FILE_READ_DIRECTORY, 0, 0, 0, "/") != 0)
//     {
//         fprintf(stderr, "send error\n");
//         return -1;
//     }

    monapi_cmemoryinfo* mi = monapi_call_file_read_directory("/", MONAPI_FALSE);
    monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
    int size = *(int*)mi->Data;
    for (int i = 0; i < size; i++, p++)
    {
        printf("%s\n", p->name);
    }
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);

    return 0;
}
