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
#include<elf.h>

extern "C" void arch_set_stack_view();
extern "C" void put_pixel(int x, int y, char color);

void syscall_entrance() {

    Thread* thread;
    Process* process;
    KMutex* mutex;
    ScreenInfo* screenInfo;
    ArchThreadInfo* info = g_currentThread->archinfo;
    dword readSize = 0;

    switch(info->ebx) {

    case SYSTEM_CALL_PRINT:
        enableInterrupt();
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

        info->eax = g_messenger->send((dword)(info->esi), (MessageInfo*)(info->ecx));
        break;

    case SYSTEM_CALL_RECEIVE:

        info->eax = g_messenger->receive(g_processManager->getCurrentProcess(), (MessageInfo*)(info->esi));
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

    case SYSTEM_CALL_LOOKUP:
        info->eax = g_processManager->lookup((char*)(info->esi));
        break;

    case SYSTEM_CALL_GET_VRAM_INFO:
        screenInfo = (ScreenInfo*)(info->esi);
        screenInfo->vram = (dword)(g_vesaDetail->physBasePtr);
        screenInfo->bpp  = (dword)(g_vesaDetail->bitsPerPixel);
        screenInfo->x    = (dword)(g_vesaDetail->xResolution);
        screenInfo->y    = (dword)(g_vesaDetail->yResolution);
        info->eax = 0;
        break;
    case SYSTEM_CALL_LOAD_PROCESS:

        enableInterrupt();
        info->eax = loadProcess(".", (char*)info->esi, false);
        break;

    case SYSTEM_CALL_MAP:

        if (!SharedMemoryObject::open(info->ecx, info->edx)) {
            info->eax = 1;
            break;
        }

        process = g_processManager->find((dword)info->esi);
        if (process == NULL) {
            info->eax = 2;
            break;
        }

        if (!SharedMemoryObject::attach(info->ecx, process, info->edi)) {
            info->eax = 3;
            break;
        }

        IOManager::grantPermission();

        info->eax = 0;
        break;

    case SYSTEM_CALL_SET_CURSOR:

        enableInterrupt();
        g_console->setCursor((int)(info->esi), (int)(info->ecx));
        info->eax = 0;
        break;

    case SYSTEM_CALL_GET_CURSOR:

        enableInterrupt();
        g_console->getCursor((int*)(info->esi), (int*)(info->ecx));
        info->eax = 0;
        break;

    case SYSTEM_CALL_FILE_OPEN:

        enableInterrupt();
        g_fdcdriver->motor(ON);
        g_fdcdriver->recalibrate();
        g_fdcdriver->recalibrate();
        g_fdcdriver->recalibrate();
        if (!g_fat12->open((char*)(info->esi), (char*)(info->ecx), FAT12::READ_MODE)) {

            info->eax = g_fat12->getErrorNo();
            break;
        }

        *((dword*)(info->edi)) = g_fat12->getFileSize();
        info->eax = 0;

        break;

    case SYSTEM_CALL_FILE_READ:

        enableInterrupt();
        info->eax = 0;
        {
            byte* buf      = (byte*)(info->esi);
            dword size     = (dword)(info->ecx);
            int readTimes  = size / 512 + (size % 512 ? 1 : 0);

            for (int i = 0; i < readTimes; i++) {

                bool readOk = g_fat12->read(buf + 512 * i);

                if (!readOk && g_fat12->getErrorNo() != FAT12::END_OF_FILE) {
                    info->eax = 1;
                    break;
                } else if (!readOk) {
                    readSize += 512;
                    info->eax = 2;
                    break;
                }
                readSize += 512;
            }
        }
        *((dword*)(info->edi)) = readSize;
        break;

    case SYSTEM_CALL_FILE_CLOSE:

        enableInterrupt();
        g_fat12->close();
        g_fdcdriver->motor(OFF);
        info->eax = 0;
        break;

    case SYSTEM_CALL_MAP_TWO:

        {
            static dword sharedId = 0x9000;
            sharedId++;

            dword attachPid      = info->esi;
            dword linearAddress1 = info->ecx;
            dword linearAddress2 = info->edi;
            dword size           = info->edx;

            enableInterrupt();

            /* process exist? */
            Process* attachProcess = g_processManager->find(attachPid);
            if (attachProcess == NULL) {
                info->eax = 1;
                break;
            }

            while (Semaphore::down(&g_semaphore_shared));
            bool isOpen    = SharedMemoryObject::open(sharedId, size);
            bool isAttaced = SharedMemoryObject::attach(sharedId, g_processManager->getCurrentProcess(), linearAddress2);
            Semaphore::up(&g_semaphore_shared);

            if (!isOpen || !isAttaced) {
                info->eax = 2;
                break;
            }

            while (Semaphore::down(&g_semaphore_shared));
            isOpen    = SharedMemoryObject::open(sharedId, size);
            isAttaced = SharedMemoryObject::attach(sharedId, attachProcess, linearAddress1);
            Semaphore::up(&g_semaphore_shared);

            if (!isOpen || !isAttaced) {
                info->eax = 3;
                break;
            }
        }
        info->eax = 0;
        break;

    default:
        g_console->printf("syscall:default");
        break;
    }

    return;
}
