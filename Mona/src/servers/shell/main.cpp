#include <monapi/messages.h>
#include <monapi/cmessage.h>

#include "Shell.h"

using namespace MonAPI;

static dword my_tid;

static void StdoutMessageLoop()
{
    Message::send(my_tid, MSG_SERVER_START_OK);

    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg) != 0) continue;

        switch (msg.header)
        {
            case MSG_PROCESS_STDOUT_DATA:
            {
                monapi_call_mouse_set_cursor(0);
                syscall_print(msg.str);
                monapi_call_mouse_set_cursor(1);
                Message::reply(&msg);
                break;
            }
        }
    }
}

int MonaMain(List<char*>* pekoe)
{
    if (!monapi_register_to_server(ID_KEYBOARD_SERVER, 1)) exit(1);
    if (!monapi_register_to_server(ID_PROCESS_SERVER, 1)) exit(1);

    /* grab stdout */
    my_tid = syscall_get_tid();
    dword id = syscall_mthread_create((dword)StdoutMessageLoop);
    syscall_mthread_join(id);
    MessageInfo msg;
    monapi_cmessage_receive_header_only(&msg, MSG_SERVER_START_OK);
    dword stdout_tid = msg.from;
    Message::sendReceive(NULL, PROCESS_STDOUT_THREAD, MSG_PROCESS_GRAB_STDOUT, stdout_tid);

    /* Server start ok */
    bool callAutoExec = true;
    dword targetID = Message::lookupMainThread("INIT");
    if (targetID == THREAD_UNKNOWN)
    {
        targetID = Message::lookupMainThread("1LINESH.SVR");
        callAutoExec = false;
    }

    /* send */
    if (targetID != THREAD_UNKNOWN && Message::send(targetID, MSG_SERVER_START_OK))
    {
        printf("%s: INIT error\n", SVR);
    }

    /* service loop */
    Shell shell(callAutoExec);
    shell.run();

    Message::sendReceive(NULL, PROCESS_STDOUT_THREAD, MSG_PROCESS_UNGRAB_STDOUT, stdout_tid);
    syscall_kill_thread(stdout_tid);
    monapi_register_to_server(ID_KEYBOARD_SERVER, 0);
    monapi_register_to_server(ID_PROCESS_SERVER, 0);
    return 0;
}
