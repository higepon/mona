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

    Thread* thread;
    KMutex* mutex;
    ArchThreadInfo* info = g_currentThread->archinfo;

    switch(info->ebx) {

    case SYSTEM_CALL_PRINT:

        g_console->printf("%s", (char*)(info->esi));

        info->eax = 0;
        break;

    case SYSTEM_CALL_PROCESS_SLEEP:

        info->eax = g_processManager->sleep(g_processManager->getCurrentProcess(), info->esi);
        break;
    case SYSTEM_CALL_KILL:
        g_processManager->kill(g_processManager->getCurrentProcess());
        schedule();
        break;

    case SYSTEM_CALL_PUT_PIXEL:

        put_pixel((int)(info->esi), (int)(info->ecx), (char)(info->edi));
        info->eax = 0;
        break;

    case SYSTEM_CALL_SEND:

        info->eax = send((char*)(info->esi), (KMessage*)(info->ecx));
        break;

    case SYSTEM_CALL_RECEIVE:

        info->eax = receive(g_processManager->getCurrentProcess(), (KMessage*)(info->esi));
        break;

    case SYSTEM_CALL_MTHREAD_CREATE:
        thread  = g_processManager->createThread(g_processManager->getCurrentProcess(), info->esi);
        info->eax = (dword)thread;
        break;

    case SYSTEM_CALL_MTHREAD_JOIN:
        g_processManager->join(g_processManager->getCurrentProcess(), (Thread*)info->esi);
        info->eax = 0;
        break;

    case SYSTEM_CALL_MUTEX_CREATE:
        mutex = new KMutex(g_processManager->getCurrentProcess());
        info->eax = mutex->init();
        break;

    case SYSTEM_CALL_MUTEX_LOCK:
        mutex = g_processManager->getCurrentProcess()->getKMutex((int)info->esi);
        if (mutex == NULL) {
            info->eax = 1;
        } else {
            info->eax = mutex->lock(g_currentThread->thread);
        }
        break;

    case SYSTEM_CALL_MUTEX_TRYLOCK:
        mutex = g_processManager->getCurrentProcess()->getKMutex((int)info->esi);
        if (mutex == NULL) {
            info->eax = 1;
        } else {
            info->eax = mutex->tryLock(g_currentThread->thread);
        }
        break;

    case SYSTEM_CALL_MUTEX_UNLOCK:
        mutex = g_processManager->getCurrentProcess()->getKMutex((int)info->esi);
        if (mutex == NULL) {
            info->eax = 1;
        } else {
            info->eax = mutex->unlock();
        }
        break;

    case SYSTEM_CALL_MUTEX_DESTROY:
        mutex = g_processManager->getCurrentProcess()->getKMutex((int)info->esi);
        if (mutex == NULL) {
            info->eax = 1;
        } else {
            delete mutex;
            info->eax = 0;
        }
        break;

    default:
        g_console->printf("syscall:default");
        break;
    }

    return;
}
