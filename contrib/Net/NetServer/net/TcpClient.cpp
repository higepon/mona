#include "TcpClient.h"

using namespace mones;
using namespace std;

TcpClient::TcpClient()
{
}

TcpClient::~TcpClient()
{
}

dword TcpClient::Connect(IPAddress ipAddress, dword port)
{
    struct uip_conn* connection = uip_connect(ipAddress.GetAddress(), HTONS(port));
    if (connection == NULL)
    {
        return 0;
    }
    TRACE("connection=%x\n", connection);
    return (dword)connection;
}

void TcpClient::Send(dword handle, vector<byte>* data)
{
    sendBuffers_[handle] = data;
}

Buffer TcpClient::Receive(dword handle)
{
    Buffer* buffer = receiveBuffers_[handle];
    if (NULL == buffer)
    {
        Buffer empty;
        return empty;
    }
    Buffer result(*buffer);
    delete buffer;
    receiveBuffers_[handle] = NULL;
    return result;
}

void TcpClient::AbortedHandler()
{
    TRACE("conn=%x %s\n", (dword)uip_conn, __FUNCTION__);
    AfterClosedHandler();
}

void TcpClient::TimeoutHandler()
{
    TRACE("conn=%x %s\n", (dword)uip_conn, __FUNCTION__);
    AfterClosedHandler();
}

void TcpClient::ClosedHandler()
{
    TRACE("conn=%x %s\n", (dword)uip_conn, __FUNCTION__);
    AfterClosedHandler();
}

void TcpClient::ConnectedHandler()
{
    TRACE("conn=%x %s\n", (dword)uip_conn, __FUNCTION__);
    dword handle = ConnectionToHandle(uip_conn);
    connected_[handle] = true;
}

void TcpClient::AckedHandler()
{
    TRACE("conn=%x %s\n", (dword)uip_conn, __FUNCTION__);
    dword handle = ConnectionToHandle(uip_conn);
    Buffer* buffer = sendBuffers_[handle];
    if (NULL == buffer) return;

    if (buffer->empty())
    {
        TRACE("Send done\n");
        return;
    }

    // erase sent data
    buffer->erase(buffer->begin(), buffer->begin() + uip_conn->len);
}

void TcpClient::NewDataHandler()
{
    if (!uip_newdata()) return;
    if (uip_datalen() == 0) return;
    printf("new data comes\n");
    Buffer* data = new Buffer(uip_appdata, &uip_appdata[uip_datalen()]);
    dword handle = ConnectionToHandle(uip_conn);
    receiveBuffers_[handle] = data;
}

void TcpClient::SendDataHandler()
{
    TRACE("conn=%x %s\n", (dword)uip_conn, __FUNCTION__);
    dword handle = ConnectionToHandle(uip_conn);
    Buffer* buffer = sendBuffers_[handle];
    if (NULL == buffer || buffer->empty())
    {
        return;
    }
    uip_send((volatile u8_t*)(&(buffer->at(0))), buffer->size());
}

void TcpClient::AfterClosedHandler()
{
}

void TcpClient::EventHandler()
{
    if (uip_aborted())
    {
        AbortedHandler();
    }
    if (uip_timedout())
    {
        TimeoutHandler();
    }
    if (uip_closed())
    {
        ClosedHandler();
    }
    if (uip_connected())
    {
        ConnectedHandler();
    }
    if (uip_acked())
    {
        AckedHandler();
    }
    if (uip_newdata())
    {
        NewDataHandler();
    }
    if (uip_rexmit() || uip_newdata() || uip_acked() || uip_acked() || uip_connected() || uip_poll())
    {
        SendDataHandler();
    }
}
