/*!
  \file  syscalls.cpp
  \brief syscalls

  Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
  All rights reserved.
  License=MIT/X Licnese

  \author  HigePon
  \version $Revision$
  \date   create:2003/03/22 update:$Date$
*/

#include<syscalls.h>
#include<global.h>
#include<tester.h>
#include<io.h>
#include<elf.h>
#include<collection.h>

void syscall_entrance() {

    Thread* thread;
    Process* process;
//    KMutex* mutex;
    ScreenInfo* screenInfo;
    ArchThreadInfo* info = g_currentThread->archinfo;

    info->eax = 0;

    switch(info->ebx) {

    case SYSTEM_CALL_PRINT:
        //enableInterrupt();
        g_console->printf("%s", (char*)(info->esi));

        info->eax = 0;
        break;

    case SYSTEM_CALL_THREAD_SLEEP:

        {
            g_scheduler->sleep(g_currentThread->thread, info->esi);
            bool isProcessChange = g_scheduler->schedule();
            ThreadOperation::switchThread(isProcessChange, 3);
        }
        break;
    case SYSTEM_CALL_KILL:

        ThreadOperation::kill();
        break;

    case SYSTEM_CALL_SEND:

        info->eax = g_messenger->send((dword)(info->esi), (MessageInfo*)(info->ecx));
        break;

    case SYSTEM_CALL_RECEIVE:

        info->eax = g_messenger->receive(g_currentThread->thread, (MessageInfo*)(info->esi));
        break;

    case SYSTEM_CALL_EXIST_MESSAGE:

        {
            bool existMessage = !(g_currentThread->thread->messageList->isEmpty());
            info->eax = existMessage ? 1 : 0;
        }
        break;

    case SYSTEM_CALL_MTHREAD_CREATE:
        thread = ThreadOperation::create(g_currentThread->process, info->esi);
        info->eax = (dword)thread;
        break;

    case SYSTEM_CALL_MTHREAD_JOIN:

        g_scheduler->join((Thread*)(info->esi));
        info->eax = 0;
        break;

    case SYSTEM_CALL_MUTEX_CREATE:
//        mutex = new KMutex(g_processManager->getCurrentProcess());
//        info->eax = mutex->init();
        break;

    case SYSTEM_CALL_MUTEX_LOCK:
//        mutex = g_processManager->getCurrentProcess()->getKMutex((int)info->esi);
//         if (mutex == NULL) {
//             info->eax = 1;
//         } else {
//             info->eax = mutex->lock(g_currentThread->thread);
//         }
        break;

    case SYSTEM_CALL_MUTEX_TRYLOCK:
//         mutex = g_processManager->getCurrentProcess()->getKMutex((int)info->esi);
//         if (mutex == NULL) {
//             info->eax = 1;
//         } else {
//             info->eax = mutex->tryLock(g_currentThread->thread);
//         }
        break;

    case SYSTEM_CALL_MUTEX_UNLOCK:
//         mutex = g_processManager->getCurrentProcess()->getKMutex((int)info->esi);
//         if (mutex == NULL) {
//             info->eax = 1;
//         } else {
//             info->eax = mutex->unlock();
//         }
        break;

    case SYSTEM_CALL_MUTEX_DESTROY:
//         mutex = g_processManager->getCurrentProcess()->getKMutex((int)info->esi);
//         if (mutex == NULL) {
//             info->eax = 1;
//         } else {
//             delete mutex;
//             info->eax = 0;
//         }
        break;

    case SYSTEM_CALL_LOOKUP:
        info->eax = g_scheduler->lookup((char*)(info->esi));
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

        {
            char* path = (char*)info->esi;
            char* name = (char*)info->ecx;
            CommandOption* option = (CommandOption*)(info->edi);

            enableInterrupt();
            info->eax = loadProcess(path, name, true, option);
            break;
        }

    case SYSTEM_CALL_MAP:

        if (!SharedMemoryObject::open(info->ecx, info->edx)) {
            info->eax = 1;
            break;
        }

        process = g_scheduler->findProcess((dword)info->esi);
        if (process == NULL) {
            info->eax = 2;
            break;
        }

        if (!SharedMemoryObject::attach(info->ecx, process, info->edi)) {
            info->eax = 3;
            break;
        }

        info->eax = 0;
        break;

    case SYSTEM_CALL_SET_CURSOR:

        //enableInterrupt();
        g_console->setCursor((int)(info->esi), (int)(info->ecx));
        info->eax = 0;
        break;

    case SYSTEM_CALL_GET_CURSOR:

        //enableInterrupt();
        g_console->getCursor((int*)(info->esi), (int*)(info->ecx));
        info->eax = 0;
        break;

    case SYSTEM_CALL_FDC_OPEN:

        enableInterrupt();
        while (Semaphore::down(&g_semaphore_fd));

        g_fdcdriver->motor(ON);
        g_fdcdriver->recalibrate();
        g_fdcdriver->recalibrate();
        g_fdcdriver->recalibrate();

        Semaphore::up(&g_semaphore_fd);
        info->eax = 0;
        break;

    case SYSTEM_CALL_FDC_CLOSE:

        enableInterrupt();
        while (Semaphore::down(&g_semaphore_fd));

        g_fdcdriver->motorAutoOff();

        Semaphore::up(&g_semaphore_fd);
        info->eax = 0;
        break;

    case SYSTEM_CALL_FDC_READ:

        enableInterrupt();

        {
            bool readResult = true;
            dword lba      = info->esi;
            byte* buffer   = (byte*)(info->ecx);
            dword blocknum = info->edi;

            while (Semaphore::down(&g_semaphore_fd));
            for (dword i = 0; i < blocknum; i++) {
                readResult = g_fdcdriver->read(lba + i, buffer + i * 512);
                if (!readResult) break;
            }
            Semaphore::up(&g_semaphore_fd);
            info->eax = readResult ? 0 : 1;
        }
        break;

    case SYSTEM_CALL_FDC_WRITE:

        enableInterrupt();
        {
            bool writeResult = true;
            dword lba      = info->esi;
            byte* buffer   = (byte*)(info->ecx);
            dword blocknum = info->edi;

            while (Semaphore::down(&g_semaphore_fd));
            for (dword i = 0; i < blocknum; i++) {
                writeResult = g_fdcdriver->write(lba + i, buffer + i * 512);
                if (!writeResult) {
                    break;
                }
            }
            Semaphore::up(&g_semaphore_fd);

            info->eax = writeResult ? 0 : 1;
        }
        break;

    case SYSTEM_CALL_FILE_OPEN:

        {
            char* path  = (char*)info->esi;
            int mode    = (int)info->ecx;
            dword* size = (dword*)info->edi;
            enableInterrupt();
            g_fdcdriver->motor(ON);
            g_fdcdriver->recalibrate();
            g_fdcdriver->recalibrate();
            g_fdcdriver->recalibrate();

            while (Semaphore::down(&g_semaphore_fd));

            if (!g_fs->open(path, mode))
            {
                info->eax = g_fs->getErrorNo();
                Semaphore::up(&g_semaphore_fd);
                break;
            }

            Semaphore::up(&g_semaphore_fd);

            *size = g_fs->size();
            info->eax = 0;
            break;
        }

    case SYSTEM_CALL_FILE_READ:

        {
            byte* buf      = (byte*)(info->esi);
            dword size     = (dword)(info->ecx);

            enableInterrupt();
            while (Semaphore::down(&g_semaphore_fd));

            if (!g_fs->read(buf, size))
            {
                info->eax = g_fs->getErrorNo();
                Semaphore::up(&g_semaphore_fd);
                break;
            }

            Semaphore::up(&g_semaphore_fd);
            disableInterrupt();
            info->eax = 0;
            break;
        }

    case SYSTEM_CALL_FILE_CLOSE:

        enableInterrupt();
        while (Semaphore::down(&g_semaphore_fd));
        g_fs->close();
        Semaphore::up(&g_semaphore_fd);

        break;

    case SYSTEM_CALL_MAP_TWO:

        {
            static dword sharedId = 0x9000;
            sharedId++;

            MappingInfo* map = (MappingInfo*)(info->esi);

            enableInterrupt();

            /* process exist? */
            Process* attachProcess = g_scheduler->findProcess(map->attachPid);
            if (attachProcess == NULL) {
                map->errorCd = 1;
                info->eax = 0;
                break;
            }

            while (Semaphore::down(&g_semaphore_shared));
            bool isOpen    = SharedMemoryObject::open(sharedId, map->size);
            bool isAttaced = SharedMemoryObject::attach(sharedId, g_currentThread->process, map->linearAddress2);
            Semaphore::up(&g_semaphore_shared);

            if (!isOpen || !isAttaced) {
                info->eax = 0;
                map->errorCd = 2;
                break;
            }

            while (Semaphore::down(&g_semaphore_shared));
            isOpen    = SharedMemoryObject::open(sharedId, map->size);
            isAttaced = SharedMemoryObject::attach(sharedId, attachProcess, map->linearAddress1);
            Semaphore::up(&g_semaphore_shared);

            if (!isOpen || !isAttaced) {
                info->eax = 0;
                map->errorCd = 3;
                break;
            }

            info->eax = sharedId;
        }
        break;

    case SYSTEM_CALL_UNMAP_TWO:

        {
            dword sharedId = info->esi;
            while (Semaphore::down(&g_semaphore_shared));
            bool result = SharedMemoryObject::detach(sharedId, g_currentThread->process);
            Semaphore::up(&g_semaphore_shared);

            info->eax = result ? 0 : 1;
        }
        break;

    case SYSTEM_CALL_GET_PID:

        info->eax = g_currentThread->process->getPid();
        break;

    case SYSTEM_CALL_GET_TID:

        info->eax = g_currentThread->thread->id;
        break;

    case SYSTEM_CALL_ARGUMENTS_NUM:

        info->eax = g_currentThread->process->getArguments()->size();
        break;

    case SYSTEM_CALL_GET_ARGUMENTS:

        {
            List<char*>* list = g_currentThread->process->getArguments();
            char* buf = (char*)(info->esi);
            int index = (int)(info->ecx);

            if (index - 1 > list->size()) {
                info->eax = 1;
                break;
            }

            strcpy(buf, list->get(index));
            info->eax = 0;
         }

        break;

    case SYSTEM_CALL_MTHREAD_YIELD_MESSAGE:

        KEvent::wait(g_currentThread->thread, KEvent::MESSAGE_COME);

        /* not reached */
        break;

    case SYSTEM_CALL_DATE:

        {
            KDate* date = (KDate*)(info->esi);
            RTC::getDate(date);
            info->eax = 0;
        }
        break;

    case SYSTEM_CALL_GET_IO:

        info->eflags = info->eflags |  0x3000;
        info->eax = 0;
        {
            bool isProcessChange = g_scheduler->setCurrentThread();
            ThreadOperation::switchThread(isProcessChange, 17);
        }
        break;

    case SYSTEM_CALL_WAIT_FDC:

        {
            if (!g_fdcdriver->interrupted())
            {
                KEvent::wait(g_currentThread->thread, KEvent::FDC_INTERRUPT);

                /* not reached */
            }
        }
        break;

    case SYSTEM_CALL_FDC_DISK_CHANGED:

        info->eax = g_fdcdriver->checkDiskChange();
        break;

    default:
        g_console->printf("syscall:default");
        break;
    }

    return;
}
