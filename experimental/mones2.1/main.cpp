#include "NicServer.h"
#include <monapi.h>
#include <monalibc.h>
#include <monalibc/stdio.h>

using namespace mones;
static NicServer* server;

void NicListenLoop();
dword nicThread;

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */
int MonaMain(List<char*>* pekoe)
{
    dword id = syscall_mthread_create((dword)NicListenLoop);
    syscall_mthread_join(id);

    for (;;) {
        if (server != NULL && server->isStarted()) {
            break;
        }
        sleep(500);
    }
    nicThread = server->getThreadID();
    //printf(">>%d\n",nicThread);
    /* Initialize the device driver. */
    //monadev_init();
    /* Initialize the uIP TCP/IP stack. */
    //uip_init();
    /* Initialize the HTTP server. */
    //httpd_init();
    //Ether ef;
    while(1) {
        sleep(500);
       // nic_read(nicThread,&ef);
       // monadev_read();
    }
    return 0;
}
/*-----------------------------------------------------------------------------------*/

void NicListenLoop()
{
    server = new NicServer;
    if (!server->initialize())
    {
        printf("NicServer initialize failed\n");
        exit(1);
    }
    server->messageLoop();
}
