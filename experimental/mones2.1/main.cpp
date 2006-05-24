#include "NetServer.h"
#include "NetClient.h"
#include <monapi.h>
#include <monalibc.h>
#include <monalibc/stdio.h>

using namespace mones;
using namespace MonAPI;

static NetServer* server;

void QuasiClientThread()
{
    while( !server->isStarted() ){
        sleep(1000);
    }
    NetClient* client = new NetClient();
    client->initalize(server->getThreadID());
    client->Example();
    delete client;
    exit(0);
}

int MonaMain(List<char*>* pekoe)
{
    server=new NetServer();
    if(!server->initialize()){
        printf("initalize failed\n");
        exit(1);
    }
    //Create Client Thread for Debug.
    server->clientid = syscall_mthread_create((dword)QuasiClientThread);
    syscall_mthread_join(server->clientid);

    server->messageLoop();
    return 0;
}
