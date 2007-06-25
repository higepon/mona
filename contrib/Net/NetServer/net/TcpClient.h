#ifndef __TCPCLIENT_H__
#define __TCPCLIENT_H__

#include <map>
#include <vector>
extern "C" {
#include <uip.h>
}
#include "IPAddress.h"

namespace mones {

//#ifdef DEBUG
#define TRACE(...) _printf(__VA_ARGS__)
//#else
//#define TRACE //
//#endif


typedef std::map<uint32_t, int> ConnectedMap;
typedef std::vector<uint8_t> Buffer;
typedef std::map<uint32_t, Buffer*> SendBuffersMap;
typedef std::map<uint32_t, Buffer*> ReceiveBuffersMap;
class TcpClient
{
public:
    TcpClient();
    virtual ~TcpClient();

    void EventHandler();
    uint32_t Connect(IPAddress ipAddress, uint32_t port);
    uint32_t ConnectionToHandle(struct uip_conn* connection)
    {
        return (uint32_t)connection;
    }
    bool Connected(uint32_t handle)
    {
        return connected_[handle];
    }
    bool HaveNewData(uint32_t handle)
    {
        return receiveBuffers_[handle] != NULL;
    }
    void Send(uint32_t handle, std::vector<uint8_t>* data);
    Buffer Receive(uint32_t handle);

protected:
    void AbortedHandler();
    void TimeoutHandler();
    void ClosedHandler();
    void ConnectedHandler();
    void AckedHandler();
    void NewDataHandler();
    void SendDataHandler();
    void AfterClosedHandler();

    ConnectedMap connected_;
    SendBuffersMap sendBuffers_;
    ReceiveBuffersMap receiveBuffers_;
};

}; // namespace mones

#endif // __TCPCLIENT_H__
