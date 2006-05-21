#pragma once
#include <sys/types.h>
namespace mones {
//'NET' is 0x4E4554
#define MSG_NET_STATUS 0x4E455401
#define MSG_NET_OPEN   0x4E455402
#define MSG_NET_CLOSE  0x4E455403
#define MSG_NET_WRITE  0x4E455404
#define MSG_NET_READ   0x4E455405

 class NETIF
 {
    word  protocol;
    word  localport;
    word  remoteport;
    dword remoteip; //dcba
    dword clientid;
 };

 };
