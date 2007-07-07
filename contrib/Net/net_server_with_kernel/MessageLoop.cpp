#include <monapi.h>
#include <monalibc.h>
#include <servers/net.h>
extern "C" {
#include "uip.h"
#include "webclient.h"
};
#include <string>

using namespace MonAPI;

MessageInfo returnMsg;

void messageLoop()
{
    for (MessageInfo msg; ;)
    {
        if (Message::receive(&msg)) continue;

        switch (msg.header)
        {
        case MSG_NET_WEB_GET:
        {
            returnMsg = msg;
            webclient_get(msg.str, 80, "/apache2-default/");
            break;
        }
        default:
            MONAPI_WARN("unknown Message comes header=%x", msg.header);
            break;
        }
    }
}

void sendWgetResult(std::string text)
{
    if (text.size() > MESSAGE_INFO_MAX_STR_LENGTH)
    {
        MONAPI_WARN("wget over flow");
    }
    char buf[MESSAGE_INFO_MAX_STR_LENGTH];
    uint32_t size = text.size() > MESSAGE_INFO_MAX_STR_LENGTH ? MESSAGE_INFO_MAX_STR_LENGTH : text.size();
    memcpy(buf, text.data(), size);
    Message::reply(&returnMsg, 0, 0, buf);
}
