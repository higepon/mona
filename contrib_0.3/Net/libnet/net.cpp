#include"net.h"
#include <servers/net.h>
#include <monapi.h>
#include <monapi/messages.h>

static const char* server = "NET.EX5";
static dword serverId;
static const dword MESSAGE_MAX = 128;

using namespace MonAPI;

void tcpInitialize()
{
    serverId = Message::lookupMainThread(server);
    serverId += 2;
}

dword tcpConnect(byte a, byte b, byte c, byte d, dword port)
{
    MessageInfo msg;
    dword ipAddress = a << 24 | b << 16 | c << 8 | d;
    if (Message::sendReceive(&msg, serverId, MSG_NET_TCP_CONNECT, ipAddress, port) != 0)
    {
        printf("can not connect to %s\n", server);
        return NULL;
    }
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    return msg.arg2;
}

dword tcpSendMemoryMap(dword handle, const byte* data, dword length)
{
    monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();
    if (!monapi_cmemoryinfo_create(mi, length, MONAPI_FALSE))
    {
        monapi_cmemoryinfo_delete(mi);
        return 1;
    }
    memcpy(mi->Data, data, length);
    MessageInfo msg;
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    if (Message::sendReceive(&msg, serverId, MSG_NET_TCP_SEND, handle, length, mi->Handle) != 0)
    {
        printf("can not connect to %s\n", server);
        monapi_cmemoryinfo_delete(mi);
        return 1;
    }
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    monapi_cmemoryinfo_delete(mi);
    // 誰が開放するの？
    // monapi_cmemoryinfo_dispose(mi);
    return 0;
}

dword tcpSend(dword handle, const byte* data, dword length)
{
    if (length > 3)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        return tcpSendMemoryMap(handle, data, length);
    }

    MessageInfo msg;
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    if (Message::sendReceive(&msg, serverId, MSG_NET_TCP_SEND, handle, length, 0, (const char*)data) != 0)
    {
        printf("can not connect to %s\n", server);
        return 1;
    }
    return 0;
}

Buffer tcpRead(dword handle)
{
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverId, MSG_NET_TCP_RECEIVE, handle) != 0)
    {
        printf("can not connect to %s\n", server);
        return Buffer(0);
    }
    if (msg.arg2 == 0)
    {
        Buffer result(msg.str, &msg.str[msg.arg3]);
        return result;
    }
    else
    {
        printf("Shared Meory Read\n");
        monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();
        mi->Handle = msg.arg2;
        mi->Size   = msg.arg3;
        monapi_cmemoryinfo_map(mi);
        Buffer result(mi->Data, &mi->Data[mi->Size]);
        return result;
    }
}
