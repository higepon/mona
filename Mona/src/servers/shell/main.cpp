#include <monapi/messages.h>
#include <monapi/cmessage.h>

#include "Shell.h"

using namespace MonAPI;

static monapi_clist msg_queue;
static dword my_tid;

static void StdoutMessageLoop()
{
    Message::send(my_tid, MSG_SERVER_START_OK);

    for (MessageInfo msg;;)
    {
        if (monapi_cmessage_receive(&msg_queue, &msg) != 0) continue;

        switch (msg.header)
        {
            case MSG_PROCESS_STDOUT_DATA:
            {
#if 1  /// temporary
                syscall_print("=");
#endif
                monapi_call_mouse_set_cursor(&msg_queue, 0);
                msg.str[127] = '\0';
#if 1  /// temporary
                syscall_print("=");
#endif
                syscall_print(msg.str);
#if 1  /// temporary
                syscall_print("=");
#endif
                monapi_call_mouse_set_cursor(&msg_queue, 1);
#if 1  /// temporary
                syscall_print("=");
#endif
                monapi_cmessage_reply(&msg);
#if 1  /// temporary
                syscall_print("=");
#endif
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
    MessageInfo msg, src;
    src.header = MSG_SERVER_START_OK;
    monapi_cmessage_receive_cond(NULL, &msg, &src, monapi_cmessage_cond_header);
    dword stdout_tid = msg.from;
    monapi_cmessage_send_receive_args(NULL, NULL, PROCESS_STDOUT_THREAD, MSG_PROCESS_GRAB_STDOUT, stdout_tid, 0, 0, NULL);

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

    monapi_cmessage_send_receive_args(NULL, NULL, PROCESS_STDOUT_THREAD, MSG_PROCESS_UNGRAB_STDOUT, stdout_tid, 0, 0, NULL);
    syscall_kill_thread(stdout_tid);
    monapi_register_to_server(ID_KEYBOARD_SERVER, 0);
    monapi_register_to_server(ID_PROCESS_SERVER, 0);
    return 0;
}
