#include "FileServer.h"
#include <map>
#include <string>

using namespace MonAPI;
using namespace std;

static volatile bool isNameServerStarted = false;

// Don't touch any files from this function, since it causes infinite loop on file server.
static void __fastcall nameServer(void* arg)
{
    isNameServerStarted = true;
    map<string, uint32_t> nameMap;
    for (;;) {
        MessageInfo msg;
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
        switch (msg.header) {
        case MSG_NAME:
            Message::reply(&msg, M_OK);
            break;
        case MSG_ADD:
            nameMap[msg.str] = msg.from;
            if (Message::reply(&msg, M_OK) != M_OK) {
                monapi_warn("reply failed");
            }
            break;
        case MSG_WHERE:
            map<string, uint32_t>::iterator it = nameMap.find(msg.str);
            if (it == nameMap.end()) {
                if (Message::reply(&msg, M_NAME_NOT_FOUND) != M_OK) {
                    monapi_warn("reply failed");
                }
            } else {
                if (Message::reply(&msg, M_OK, (*it).second) != M_OK) {
                    monapi_warn("reply failed");
                }
            }
            break;
        }
    }
    exit(0);
}

int main(int argc, char* argv[])
{
    // Currently name server is running on file server for dependency problem.
    // Don't use monapi_thread_create, not to register the thread to process server.
    syscall_mthread_create_with_arg(nameServer, NULL);
    while (!isNameServerStarted );

    if (monapi_name_add("/servers/file") != M_OK) {
        monapi_fatal("monapi_name_add failed");
    }

    if (monapi_notify_server_start("INIT") != M_OK) {
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
