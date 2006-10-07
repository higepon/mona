#include "stdio.h"
#include "FileServer.h"

using namespace MonAPI;

// how to check memory leak of file_server
// modify messages.cpp
// valgrind --leak-check=full -v --show-reachable=yes ./file_server 2> val.log


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
