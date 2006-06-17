#include "stdio.h"
#include "FileServer.h"

using namespace MonAPI;

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
    FileServer server;
    if (server.initializeFileSystems() != MONA_SUCCESS)
    {
        printf("fileserver filesystem initialize error\n");
        exit(1);
    }
    server.messageLoop();

    return 0;
}
