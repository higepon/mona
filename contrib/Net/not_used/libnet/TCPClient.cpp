#include "TCPClient.h"
#include <servers/net.h>

using namespace MonAPI;
using namespace net;

TCPClient::TCPClient() : netServer_("NET.EX5"), serverId_(THREAD_UNKNOWN), handle_(0)
{
    initialize();
}

TCPClient::~TCPClient()
{
}

void TCPClient::initialize()
{
    serverId_ = Message::lookupMainThread(netServer_);
    serverId_ += 2; // Bad!
}

bool TCPClient::connect(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint32_t port)
{
    MessageInfo msg;
    uint32_t ipAddress = a << 24 | b << 16 | c << 8 | d;
    if (Message::sendReceive(&msg, serverId_, MSG_NET_TCP_CONNECT, ipAddress, port) != 0)
    {
        printf("can not connect to %s\n", netServer_);
        return false;
    }
    handle_ = msg.arg2;
    return true;
}

uint32_t TCPClient::writeByMap(const uint8_t* data, uint32_t length)
{
    monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();
    if (!monapi_cmemoryinfo_create(mi, length, MONAPI_FALSE))
    {
        monapi_cmemoryinfo_delete(mi);
        return 1;
    }
    memcpy(mi->Data, data, length);
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverId_, MSG_NET_TCP_SEND, handle_, length, mi->Handle) != 0)
    {
        printf("can not connect to %s\n", netServer_);
        monapi_cmemoryinfo_delete(mi);
        return 1;
    }
    monapi_cmemoryinfo_delete(mi);
    // 誰が開放するの？
    // monapi_cmemoryinfo_dispose(mi);
    return 0;
}


uint32_t TCPClient::write(const uint8_t* data, uint32_t length)
{
    if (length > 3)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        return writeByMap(data, length);
    }

    MessageInfo msg;
    if (Message::sendReceive(&msg, serverId_, MSG_NET_TCP_SEND, handle_, length, 0, (const char*)data) != 0)
    {
        printf("can not connect to %s\n", netServer_);
        return 1;
    }
    return 0;
}

Buffer TCPClient::read()
{
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverId_, MSG_NET_TCP_RECEIVE, handle_) != 0)
    {
        printf("can not connect to %s\n", netServer_);
        return Buffer(0);
    }
    if (msg.arg2 == 0)
    {
        Buffer result(msg.str, &msg.str[msg.arg3]);
        return result;
    }
    else
    {
        monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();
        mi->Handle = msg.arg2;
        mi->Size   = msg.arg3;
        monapi_cmemoryinfo_map(mi);
        Buffer result(mi->Data, &mi->Data[mi->Size]);
        return result;
    }
}

bool TCPClient::isClosed()
{
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverId_, MSG_NET_TCP_IS_CLOSED, handle_) != 0)
    {
        printf("can not connect to %s\n", netServer_);
        return false;
    }
    return msg.arg2 == 1;
}
