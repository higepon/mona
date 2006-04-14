#ifndef __TCPCLIENT_H__
#define __TCPCLIENT_H__

#include <monapi.h>
#include <monapi/messages.h>
#include <vector>

namespace net {

typedef std::vector<byte> Buffer;

class TCPClient
{
public:
    TCPClient();
    virtual ~TCPClient();

    bool connect(byte a, byte b, byte c, byte d, dword port);
    dword write(const byte* data, dword length);
    Buffer read();
    bool isClosed();

protected:
    void initialize();
    dword writeByMap(const byte* data, dword length);
    const char* netServer_;
    dword serverId_;
    dword handle_;
};

}; // namespace net

#endif // __TCPCLIENT_H__
