#include <monapi/messages.h>

#include "Shell.h"

using namespace MonAPI;

int MonaMain(List<char*>* pekoe)
{
    if (!monapi_register_to_server(ID_KEYBOARD_SERVER, 1)) exit(1);
    if (!monapi_register_to_server(ID_PROCESS_SERVER, 1)) exit(1);
    bool callAutoExec = true;

    /* Server start ok */
    dword targetID = Message::lookupMainThread("INIT");
    if (targetID == THREAD_UNKNOWN)
    {
        targetID = Message::lookupMainThread("1LINESH.SVR");
        if (targetID == THREAD_UNKNOWN)
        {
            printf("%s: INIT not found\n", SVR);
            exit(1);
        }
        callAutoExec = false;
    }

    /* send */
    if (Message::send(targetID, MSG_SERVER_START_OK))
    {
        printf("%s: INIT error\n", SVR);
    }

    /* service loop */
    Shell shell(callAutoExec);
    shell.run();

    monapi_register_to_server(ID_KEYBOARD_SERVER, 0);
    monapi_register_to_server(ID_PROCESS_SERVER, 0);
    return 0;
}
