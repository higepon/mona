#include <monapi.h>
#include <monalibc.h>
#include <servers/net.h>
extern "C" {
#include "uip.h"
#include "webclient.h"
};
#include <string>
#include "uipLoop.h"

using namespace MonAPI;
using namespace std;

extern WGetInfo wgetinfo;

void messageLoop()
{
    for (MessageInfo msg; ;)
    {
        if (Message::receive(&msg)) continue;

        switch (msg.header)
        {
        case MSG_NET_WEB_GET:
        {
            string url = msg.str;
            uint32_t ipaddr[4];
            char host[32];
            char path[32];
            sscanf(msg.str, "http://%d.%d.%d.%d%s", &ipaddr[0], &ipaddr[1], &ipaddr[2], &ipaddr[3], path);
            sprintf(host, "%d.%d.%d.%d",  ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
//            _printf("host=%s path=%s\n", host, path);
            wgetinfo.from = msg.from;
            wgetinfo.host = host;
            wgetinfo.port = 80;
            wgetinfo.path = path;
            wgetinfo.exec = true;
            break;
        }
        default:
            MONAPI_WARN("unknown Message comes header=%x", msg.header);
            break;
        }
    }
}

void sendWebGetResult(const std::string& text)
{
    int textSize = text.size();
    int restSize = textSize;
    const char* p = text.data();
    char buf[MESSAGE_INFO_MAX_STR_LENGTH];

    if (textSize == 0)
    {
        Message::send(wgetinfo.from, MSG_RESULT_ERROR);
        return;
    }
    else
    {
        Message::send(wgetinfo.from, MSG_RESULT_OK);
    }

    while (restSize > 0)
    {
        // MessageInfo format
        //   arg1 = length
        //   arg2 = 1(have more content) or  0(no more content)
        if (restSize > MESSAGE_INFO_MAX_STR_LENGTH)
        {
            memcpy(buf, p + (textSize - restSize), MESSAGE_INFO_MAX_STR_LENGTH);
            Message::send(wgetinfo.from, MSG_NET_WEB_GET, MESSAGE_INFO_MAX_STR_LENGTH, 1, 0, buf);
            restSize -= MESSAGE_INFO_MAX_STR_LENGTH;
        }
        else
        {
            memcpy(buf, p + (textSize - restSize), restSize);
            Message::send(wgetinfo.from, MSG_NET_WEB_GET, restSize, 0, 0, buf);
            restSize = 0;
        }
    }
}
