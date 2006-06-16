#include "stdio.h"
#include "file_server.h"

using namespace MonAPI;

VnodeManager* vmanager;

void MessageLoop()
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg)) continue;

        switch (msg.header)
        {
        case MSG_VFS_FILE_OPEN:
        {
            dword tid = msg.from; // temporary
            dword fildID;
            int ret = vmanager->open(msg.str, 0, false, tid, &fildID);
            Message::reply(&msg, ret == MONA_SUCCESS ? fildID : MONA_FAILURE);
            break;
        }
        default:
        {
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
    MessageLoop();
    return 0;
}
