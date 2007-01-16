#ifndef __TCPCLIENT_H__
#define __TCPCLIENT_H__

#include <monapi.h>
#include <monapi/messages.h>
#include <vector>

namespace net {

typedef std::vector<uint8_t> Buffer;

class TCPClient
{
public:
    TCPClient();
    virtual ~TCPClient();

    bool connect(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint32_t port);
    uint32_t write(const uint8_t* data, uint32_t length);
    Buffer read();
    bool isClosed();

protected:
    void initialize();
    uint32_t writeByMap(const uint8_t* data, uint32_t length);
    const char* netServer_;
    uint32_t serverId_;
    uint32_t handle_;
};

}; // namespace net

#endif // __TCPCLIENT_H__
