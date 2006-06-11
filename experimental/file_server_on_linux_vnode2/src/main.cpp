#include "stdio.h"
#include "Message.h"
#include "vnode.h"

using namespace MonAPI;

VnodeManager* vmanager;

void MessageLoop()
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg)) continue;

        switch (msg.header)
        {
        default:
            break;
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
