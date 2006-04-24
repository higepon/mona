#include <stdio.h>
#include "types.h"
#include "Message.h"
#include "servers/file.h"

using namespace MonAPI;

int main()
{
    Message::initialize();
    dword id = Message::lookupMainThread("file_server");
    if (id == THREAD_UNKNOWN)
    {
        fprintf(stderr, "file_server not found\n");
        return -1;
    }
    MessageInfo msg;
    if (Message::sendReceive(&msg, id, MSG_FILE_READ_DIRECTORY, 0, 0, 0, "/") != 0)
    {
        fprintf(stderr, "send error\n");
        return -1;
    }


    return 0;
}
