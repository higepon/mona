#include <monapi/messages.h>
#include <monapi.h>
#include <stack>

using namespace MonAPI;
using namespace std;
int main(int argc, char* argv[])
{
    if (monapi_notify_server_start("MONITOR.BIN") != M_OK) {
        monapi_warn("MONITOR not found");
        exit(-1);
    }

    stack<monapi_cmemoryinfo> clips;

    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg) != 0) continue;

        switch (msg.header)
        {
        case MSG_CLIPBOARD_PUSH:
        {
            monapi_cmemoryinfo memory;
            memory.Handle = msg.arg1;
            memory.Owner  = msg.from;
            memory.Size   = msg.arg2;
            ASSERT(memory.Handle != 0);

            // Use immediate map
            intptr_t result = monapi_cmemoryinfo_map(&memory, true);
            if (result != M_OK) {
                Message::replyError(&msg, result);
            } else {
                clips.push(memory);
                Message::reply(&msg);
            }
            break;
        }
        case MSG_CLIPBOARD_POP:
            if (clips.empty()) {
                Message::replyError(&msg, M_OBJECT_NOT_FOUND);
            } else {
                monapi_cmemoryinfo memory = clips.top();
                clips.pop();
                Message::reply(&msg, memory.Handle, memory.Size);
            }
            break;
        default:
            break;
        }
    }

    return 0;
}
