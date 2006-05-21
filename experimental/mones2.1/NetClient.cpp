#include "NetClient.h"
#include "NetServer.h"

using namespace MonAPI;
using namespace mones;

int NetClient::Check()
{
    //Send Test Messages.
    //1. open
    MessageInfo info;
    Message::create(&info,MSG_NET_OPEN, 0, 0, 0, NULL);
    if (Message::send(serverid, &info)) {
        printf("MSG_NET_OPEN error\n");
    }
    //2. send 

    //3. recv

    //4. close

    return 0;
}

int NetClient::initalize(dword threadid)
{
    serverid=threadid; 
    clientid= System::getThreadID();
    return 0;
}

dword NetClient::MessageLoop()
{
    for(MessageInfo msg;;)
    {
        if (Message::receive(&msg)) continue;
        switch (msg.header)
        {
        case MSG_NET_OPEN:
            break;
        default:
            printf("Client default: %d", msg.header);
            break;
        }
    }
    printf("ClientExit\n");
    return 0;
}

/*
    if (this->frameList.size() == 0){
        MessageInfo m;
        for (int i = 0; ; i++){
            int result = MonAPI::Message::peek(&m, i);
            if (result != 0){
                i--;
                syscall_mthread_yield_message();
            } else if (m.header == MSG_INTERRUPTED) {
                printf("XX\n");
                MonAPI::Message::peek(&m, i, PEEK_REMOVE);
                interrupt(&m);
                break;
            }
         }
     }
*/
            
//Ether::Frame* frame = this->frameList.removeAt(0);
//SetFrameToSharedMemory(frame);
//delete frame; 
