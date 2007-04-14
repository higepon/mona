#include <monapi/messages.h>
#include <servers/screen.h>

#include "Shell.h"

using namespace MonAPI;

// static uint32_t my_tid;

// static void StdoutMessageLoop()
// {
//     Message::send(my_tid, MSG_SERVER_START_OK);

//     for (MessageInfo msg;;)
//     {
//         if (Message::receive(&msg) != 0) continue;

//         switch (msg.header)
//         {
//             case MSG_PROCESS_STDOUT_DATA:
//             {
// #if 0  /// DEBUG for message
//                 char buf[256];
//                 sprintf(buf, "!%d!", msg.from);
//                 syscall_print(buf);
// #endif
//                 monapi_call_mouse_set_cursor(0);
//                 msg.str[127] = '\0';
//                 syscall_print(msg.str);
//                 monapi_call_mouse_set_cursor(1);
// #if 0  /// DEBUG for message
//                 syscall_print("!E!");
// #endif
//                 Message::reply(&msg);
//                 break;
//             }
//         }
//     }
// }

int main(int argc, char* argv[])
{
    if (!monapi_register_to_server(ID_KEYBOARD_SERVER, 1)) exit(1);
    if (!monapi_register_to_server(ID_PROCESS_SERVER, 1)) exit(1);

//     /* grab stdout */
//     my_tid = syscall_get_tid();
//     uint32_t id = syscall_mthread_create((uint32_t)StdoutMessageLoop);
//     MessageInfo msg, src;
//     src.header = MSG_SERVER_START_OK;
//     Message::receive(&msg, &src, Message::equalsHeader);
//     uint32_t stdout_tid = msg.from;
//     uint32_t svr_tid = monapi_get_server_thread_id(ID_PROCESS_SERVER);
//     if (svr_tid != THREAD_UNKNOWN)
//     {
//         Message::sendReceive(NULL, svr_tid + 1, MSG_PROCESS_GRAB_STDOUT, stdout_tid);
//     }

    /* Server start ok */
//    bool callAutoExec = true;
    if (MONAPI_FALSE == monapi_notify_server_start("MONITOR.BIN"))
    {
//        callAutoExec = false;
        printf("MONITOR not found\n");
        exit(-1);
    }
    MessageInfo msg;
    uint32_t targetID = Message::lookupMainThread("SCREEN.EX5");
    if (targetID == THREAD_UNKNOWN || Message::sendReceive(&msg, targetID, MSG_SCREEN_GET_STREAM_HANDLE)) {
        printf("SCREEN.EX5 not found\n");
        exit(-1);
    }
    uint32_t screenHandle = msg.arg2;

    /* service loop */
    Shell shell(screenHandle);
    shell.run();

//     if (svr_tid != THREAD_UNKNOWN)
//     {
//         Message::sendReceive(NULL, svr_tid + 1, MSG_PROCESS_UNGRAB_STDOUT, stdout_tid);
//     }
//    syscall_kill_thread(stdout_tid);
    monapi_register_to_server(ID_KEYBOARD_SERVER, 0);
    monapi_register_to_server(ID_PROCESS_SERVER, 0);
    return 0;
}
