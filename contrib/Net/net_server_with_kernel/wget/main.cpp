#include <monapi.h>
#include <servers/net.h>
#include <limits.h>
#include <stdio.h>
#include <string>

using namespace MonAPI;
using namespace std;

void receiveContent(string& content);

int main(int argc, char* argv[])
{
    if (argc < 1)
    {
        fprintf(stderr, "usage: wget http://.....\n");
        return -1;
    }

    const char* url = argv[1];
    int len = strlen(url);
    if (len > MESSAGE_INFO_MAX_STR_LENGTH)
    {
        fprintf(stderr, "URL is too long, max %d bytes\n", MESSAGE_INFO_MAX_STR_LENGTH);
        return -1;
    }

    MessageInfo msg;
    if (Message::sendReceive(&msg, monapi_get_server_thread_id(ID_NET_SERVER),
                             MSG_NET_WEB_GET,
                             0,
                             0,
                             0,
                             url) != 0)
    {
        fprintf(stderr, "NET server not found\n");
        return -1;
    }

    if (msg.arg2 != MONA_SUCCESS)
    {
        fprintf(stderr, "error\n");
        return -1;
    }

    string content("");
    receiveContent(content);

    printf(content.data());

    return 0;
}

void receiveContent(string& content)
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg)) continue;

        switch (msg.header)
        {
        case MSG_NET_WEB_GET:
        {
            // arg1 = length
            // arg2 = 1(have more content) or  0(no more content)
            content += string(msg.str, msg.arg1);
            if (msg.arg2 == 0) return;
            break;
        }
        default:
            MONAPI_WARN("unknown Message comes header=%x", msg.header);
            break;
        }
    }
}
