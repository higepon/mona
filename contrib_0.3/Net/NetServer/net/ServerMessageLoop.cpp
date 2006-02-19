extern "C" {
#include <uip.h>
}

#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include <servers/net.h>
#include "assert.h"
#include "ServerMessageLoop.h"
#include "TcpClient.h"

using namespace mones;
using namespace MonAPI;
using namespace std;

static TcpClient* client;

void MessageLoop()
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg)) continue;

        switch (msg.header)
        {
            case MSG_NET_TCP_CONNECT:
            {
                dword ipAddress = msg.arg1;
                dword port = msg.arg2;
                byte a = (ipAddress >> 24) & 0xff;
                byte b = (ipAddress >> 16) & 0xff;
                byte c = (ipAddress >> 8) & 0xff;
                byte d = (ipAddress & 0xff);
                printf("connect to %d.%d.%d.%d\n", a ,b , c, d);
                dword handle = client->Connect(IPAddress(a, b, c, d), port);
                Message::reply(&msg, handle);
                break;
            }
            case MSG_NET_TCP_SEND:
            {
                dword handle = msg.arg1;
                dword length = msg.arg2;
                dword mapHandle = msg.arg3;

                // dame dame
                dword i;
                while (!client->Connected(handle))
                {
                    for (int j = 0; j < 10000; j++)
                    {
                        i *= 0;
                    }
                }

                if (mapHandle != 0)
                {
                    monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();
                    mi->Handle = mapHandle;
                    mi->Size   = length;
                    monapi_cmemoryinfo_map(mi);
                    Buffer data(mi->Data, &mi->Data[length]);
                    client->Send(handle, &data);
                    Message::reply(&msg);
                    break;
                } else
                {
                    Buffer data(msg.str, &msg.str[length]);
                    client->Send(handle, &data);
                    Message::reply(&msg);
                    break;
                }
            }
            case MSG_NET_TCP_RECEIVE:
            {
                dword handle = msg.arg1;
                dword i;
                while (!client->HaveNewData(handle))
                {
                    for (int j = 0; j < 10000; j++)
                    {
                        i *= 0;
                    }
                }
                Buffer buffer = client->Receive(handle);
                dword bufferSize = buffer.size();
                if (bufferSize > 128)
                {
                    monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();
                    if (!monapi_cmemoryinfo_create(mi, bufferSize, MONAPI_FALSE))
                    {
                        monapi_cmemoryinfo_delete(mi);
                        printf("unable to monapi_cmemoryinfo_create mi\n");
                        break;
                    }
                    memcpy(mi->Data, &buffer[0], bufferSize);
                    Message::reply(&msg, mi->Handle, mi->Size);
                    monapi_cmemoryinfo_delete(mi);
                }
                else
                {
                    Message::reply(&msg, 0, bufferSize, (char*)&buffer[0]);
                }
                break;
            }
        }

    }
}

void server_init(void)
{
    client = new TcpClient();
}

void server_appcall(void)
{
    client->EventHandler();
}
