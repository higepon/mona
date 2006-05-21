#include "NetServer.h"
#include "NetClient.h"
#include <monapi.h>
#include <monalibc.h>
#include <monalibc/stdio.h>

using namespace mones;
using namespace MonAPI;

static NetServer* server;

dword QuasiClientThread()
{
    while( !server->isStarted() ){
        sleep(1000);
    }
    NetClient* client = new NetClient();
    client->initalize(server->getThreadID());
    client->Test();
    return 0;
}

int MonaMain(List<char*>* pekoe)
{
    server=new NetServer();
    if(!server->initialize()){
        printf("initalize failed\n");
        exit(1);
    }
    //Create Client Thread for Debug.
    dword id = syscall_mthread_create((dword)QuasiClientThread);
    syscall_mthread_join(id);

    server->messageLoop();
    return 0;
}
