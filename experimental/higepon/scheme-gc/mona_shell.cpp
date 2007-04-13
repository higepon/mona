#ifdef MONA

#include <monapi.h>
#include <servers/screen.h>
#include "mona_shell.h"

using namespace MonAPI;

int mona_shell_init()
{
    if (!monapi_register_to_server(ID_KEYBOARD_SERVER, 1))
    {
        printf("register to keyboard server failed\n");
        return -1;
    }

    MessageInfo msg;
    uint32_t targetID = Message::lookupMainThread("SCREEN.EX5");
    if (targetID == THREAD_UNKNOWN || Message::sendReceive(&msg, targetID, MSG_SCREEN_GET_STREAM_HANDLE)) {
        printf("SCREEN.EX5 not found\n");
        return -1;
    }
    uint32_t screenHandle = msg.arg2;


    monapi_register_to_server(ID_PROCESS_SERVER, 0);
    return 0;

}

int mona_shell_fini()
{
    monapi_register_to_server(ID_KEYBOARD_SERVER, 0);
    return 0;
}

#else
int mona_shell_init()
{
}

int mona_shell_fini()
{
}

#endif
