/*!
    COPYRIGHT AND PERMISSION NOTICE

    Copyright (c) 2002-2007 Higepon

    All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished to
    do so, provided that the above copyright notice(s) and this permission
    notice appear in all copies of the Software and that both the above
    copyright notice(s) and this permission notice appear in supporting
    documentation.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE
    LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR
    ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
    IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
    OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

    Except as contained in this notice, the name of a copyright holder shall not
    be used in advertising or otherwise to promote the sale, use or other
    dealings in this Software without prior written authorization of the
    copyright holder.
*/

/*!
    \file   main.cpp
    \brief  wget

    Copyright (c) 2002-2007 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision: 4427 $
    \date   create:2007/07/08 update:$Date $
*/

#include <monapi.h>
#include <servers/net.h>
#include <limits.h>
#include <stdio.h>
#include <string>

using namespace MonAPI;
using namespace std;

int receiveContent(string& content);

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

    // don't use sendReceive, because a reply comes from other thread of net server!
    if (Message::send(monapi_get_server_thread_id(ID_NET_SERVER),
                      MSG_NET_WEB_GET,
                      0,
                      0,
                      0,
                      url) != 0)
    {
        fprintf(stderr, "NET server not found\n");
        return -1;
    }

    string content("");
    if (MONA_FAILURE == receiveContent(content))
    {
        fprintf(stderr, "wget error\n");
        return -1;
    }
    printf(content.data());
    return 0;
}

int receiveContent(string& content)
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg)) continue;

        switch (msg.header)
        {
        case MSG_RESULT_ERROR:
            return MONA_FAILURE;
        case MSG_RESULT_OK:
            break;
        case MSG_NET_WEB_GET:
        {
            // MessageInfo format
            //   arg1 = length
            //   arg2 = 1(have more content) or  0(no more content)
            content += string(msg.str, msg.arg1); 
            if (0 == msg.arg2)
            {
                return MONA_SUCCESS;
            }
            break;
        }
        default:
            MONAPI_WARN("unknown Message comes header=%x", msg.header);
            break;
        }
    }
}
