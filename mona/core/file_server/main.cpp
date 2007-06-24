#include "FileServer.h"

using namespace MonAPI;

int main(int argc, char* argv[])
{
    if (MONAPI_FALSE == monapi_notify_server_start("INIT"))
    {
        exit(-1);
    }

    FileServer server;
    if (server.initializeFileSystems() != MONA_SUCCESS)
    {
        _printf("fileserver filesystem initialize error\n");
        exit(1);
    }
    server.messageLoop();

    return 0;
}
