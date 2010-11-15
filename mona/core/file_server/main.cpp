#include "FileServer.h"

using namespace MonAPI;

int main(int argc, char* argv[])
{
    if (monapi_notify_server_start("INIT") != M_OK)
    {
        exit(-1);
    }

    // On file server, we can't read map file.

    FileServer server;
    if (server.initializeFileSystems() != M_OK)
    {
        _printf("fileserver filesystem initialize error\n");
        exit(1);
    }
    server.messageLoop();

    return 0;
}
