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
//#define TRACE(...) printf(__VA_ARGS__)
//#else
#define TRACE //
//#endif


typedef std::map<dword, int> ConnectedMap;
typedef std::vector<byte> Buffer;
typedef std::map<dword, Buffer*> SendBuffersMap;
typedef std::map<dword, Buffer*> ReceiveBuffersMap;
class TcpClient
{
public:
    TcpClient();
    virtual ~TcpClient();

    void EventHandler();
    dword Connect(IPAddress ipAddress, dword port);
    dword ConnectionToHandle(struct uip_conn* connection)
    {
        return (dword)connection;
    }
    bool Connected(dword handle)
    {
        return connected_[handle];
    }
    bool HaveNewData(dword handle)
    {
        return receiveBuffers_[handle] != NULL;
    }
    void Send(dword handle, std::vector<byte>* data);
    Buffer Receive(dword handle);

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
