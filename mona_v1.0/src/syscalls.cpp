/*!
  \file  syscalls.cpp
  \brief syscalls

  Copyright (c) 2002,2003 Higepon
  All rights reserved.
  License=MIT/X Licnese

  \author  HigePon
  \version $Revision$
  \date   create:2003/03/22 update:$Date$
*/

#include<syscalls.h>
#include<userlib.h>
#include<global.h>
#include<tester.h>
#include<io.h>

extern "C" char pos_x;
extern "C" char pos_y;

extern "C" void arch_set_stack_view();
extern "C" void put_pixel(int x, int y, char color);

void syscall_entrance() {

    int x, y;
    Thread* thread;
    ArchThreadInfo* info = g_currentThread->archinfo;

    switch(info->ebx) {

    case SYSTEM_CALL_PRINT:

        while (Semaphore::down(&g_semaphore_console));

        x = pos_x;
        y = pos_y;
        pos_x = 1, pos_y = 27;

        g_console->printf("user:stdout[%s]", (char*)(info->esi));

        pos_x = x;
        pos_y = y;

        Semaphore::up(&g_semaphore_console);

        break;

    case SYSTEM_CALL_PROCESS_SLEEP:

        info->eax = g_processManager->sleep(g_processManager->getCurrentProcess(), info->esi);
        break;
    case SYSTEM_CALL_KILL:

        g_processManager->kill(g_processManager->getCurrentProcess());
        break;

    case SYSTEM_CALL_PUT_PIXEL:

        put_pixel((int)(info->esi), (int)(info->ecx), (char)(info->edi));
        info->eax = 0;
        break;

    case SYSTEM_CALL_SEND:

        info->eax = send((char*)(info->esi), (Message*)(info->ecx));
        break;

    case SYSTEM_CALL_RECEIVE:

        info->eax = receive(g_processManager->getCurrentProcess(), (Message*)(info->esi));
        break;

    case SYSTEM_CALL_MTHREAD_CREATE:
        thread  = g_processManager->createThread(g_processManager->getCurrentProcess(), info->esi);
        info->eax = (dword)thread;
        break;

    case SYSTEM_CALL_MTHREAD_JOIN:
        g_processManager->join(g_processManager->getCurrentProcess(), (Thread*)info->esi);
        info->eax = 0;
        break;

    default:
        g_console->printf("syscall:default");
        break;
    }
    return;
}
