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

#include "syscalls.h"
#include "global.h"
#include "tester.h"
#include "io.h"
#include "Loader.h"

extern const char* version;
extern dword version_number;

dword systemcall_mutex_create()
{
    KMutex* mutex = new KMutex();
    return g_id->allocateID(mutex);
}

dword systemcall_mutex_lock(dword id)
{
    dword result;
    SYSCALL_1(SYSTEM_CALL_MUTEX_LOCK, result, id);
    return result;
}

dword systemcall_mutex_unlock(dword id)
{
   int result;
    SYSCALL_1(SYSTEM_CALL_MUTEX_UNLOCK, result, id);
    return result;
}

// don't call without systemcall
// this has context change
// use systemcall_mutex_lock()
static dword systemcall_mutex_lock2(dword id)
{
    KObject* object = g_id->get(id, g_currentThread->thread);

    if (object == NULL)
    {
        return g_id->getLastError();
    }
    else if (object->getType() != KObject::KMUTEX)
    {
        return (dword)-10;
    }

    return ((KMutex*)object)->lock(g_currentThread->thread);
}

// don't call without systemcall
// this has context change
// use systemcall_mutex_unlock()
static dword systemcall_mutex_unlock2(dword id)
{
    KObject* object = g_id->get(id, g_currentThread->thread);

    if (object == NULL)
    {
        return g_id->getLastError();
    }
    else if (object->getType() != KObject::KMUTEX)
    {
        return (dword)-10;
    }

    return ((KMutex*)object)->unlock();
}

void syscall_entrance() {

    ScreenInfo* screenInfo;
    ArchThreadInfo* info = g_currentThread->archinfo;

    info->eax = 0;

    switch(info->ebx) {

    case SYSTEM_CALL_PRINT:
        //enableInterrupt();

        g_console->printf("%s", (char*)(info->esi));

        info->eax = 0;
        break;

    case SYSTEM_CALL_MTHREAD_SLEEP:

        {
            g_scheduler->Sleep(g_currentThread->thread, info->esi);
            g_scheduler->SwitchToNext();
        }
        break;
    case SYSTEM_CALL_KILL:

        ThreadOperation::kill();
        break;

    case SYSTEM_CALL_KILL_THREAD:

        {
            dword tid = info->esi;
            info->eax = ThreadOperation::kill(tid);
            break;
        }

    case SYSTEM_CALL_SEND:

        info->eax = g_messenger->send((dword)(info->esi), (MessageInfo*)(info->ecx));
        g_scheduler->SwitchToNext();

        /* not reaced */

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

        {
            Thread* thread = ThreadOperation::create(g_currentThread->process, info->esi);
            info->eax = g_id->allocateID(thread);
            break;
        }

    case SYSTEM_CALL_MTHREAD_JOIN:

        {
            KObject* object = g_id->get(info->esi, g_currentThread->thread);

            if (object == NULL)
                {
                    info->eax = g_id->getLastError();
                }
            else if (object->getType() != KObject::THREAD)
                {
                    info->eax = (dword)-10;
                }
            else
                {
                    g_scheduler->Join((Thread*)object);
                }

        }
        break;

    case SYSTEM_CALL_MUTEX_CREATE:

        info->eax = systemcall_mutex_create();
        break;

    case SYSTEM_CALL_MUTEX_LOCK:

        info->eax = systemcall_mutex_lock2(info->esi);
        break;

    case SYSTEM_CALL_MUTEX_TRYLOCK:

        {
            KObject* object = g_id->get(info->esi, g_currentThread->thread);

            if (object == NULL)
                {
                    info->eax = g_id->getLastError();
                }
            else if (object->getType() != KObject::KMUTEX)
                {
                    info->eax = (dword)-10;
                }
            else
                {
                    info->eax = ((KMutex*)object)->tryLock(g_currentThread->thread);
                }
        }
        break;

    case SYSTEM_CALL_MUTEX_UNLOCK:

        info->eax = systemcall_mutex_unlock2(info->esi);
        break;

    case SYSTEM_CALL_MUTEX_DESTROY:

        {
            KObject* object = g_id->get(info->esi, g_currentThread->thread);

            if (object == NULL)
                {
                    info->eax = g_id->getLastError();
                }
            else if (object->getType() != KObject::KMUTEX)
                {
                    info->eax = (dword)-10;
                }
            else
                {
                    KMutex* mutex = (KMutex*)object;
                    delete mutex;
                    info->eax = 0;
                }
        }
        break;

    case SYSTEM_CALL_LOOKUP:
        info->eax = g_scheduler->Lookup((char*)(info->esi));
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
            //            info->eax = loadProcess(path, name, true, option);
            info->eax = Loader::Load(path, name, true, option);
            break;
        }

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
        systemcall_mutex_lock(g_mutexFloppy);

        g_fdcdriver->motor(ON);
        g_fdcdriver->recalibrate();
        g_fdcdriver->recalibrate();
        g_fdcdriver->recalibrate();

        systemcall_mutex_unlock(g_mutexFloppy);
        info->eax = 0;
        break;

    case SYSTEM_CALL_FDC_CLOSE:

        enableInterrupt();
        systemcall_mutex_lock(g_mutexFloppy);

        g_fdcdriver->motorAutoOff();

        systemcall_mutex_unlock(g_mutexFloppy);
        info->eax = 0;
        break;

    case SYSTEM_CALL_FDC_READ:

        enableInterrupt();

        {
            bool readResult = true;
            dword lba      = info->esi;
            byte* buffer   = (byte*)(info->ecx);
            dword blocknum = info->edi;

            systemcall_mutex_lock(g_mutexFloppy);
            for (dword i = 0; i < blocknum; i++) {
                readResult = g_fdcdriver->read(lba + i, buffer + i * 512);
                if (!readResult) break;
            }
            systemcall_mutex_unlock(g_mutexFloppy);
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

            systemcall_mutex_lock(g_mutexFloppy);
            for (dword i = 0; i < blocknum; i++) {
                writeResult = g_fdcdriver->write(lba + i, buffer + i * 512);
                if (!writeResult) {
                    break;
                }
            }
            systemcall_mutex_unlock(g_mutexFloppy);

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

            systemcall_mutex_lock(g_mutexFloppy);

            if (!g_fs->open(path, mode))
                {
                    info->eax = g_fs->getErrorNo();
                    g_fdcdriver->motorAutoOff();
                    systemcall_mutex_unlock(g_mutexFloppy);
                    break;
                }

            systemcall_mutex_unlock(g_mutexFloppy);

            *size = g_fs->size();
            info->eax = 0;
            break;
        }

    case SYSTEM_CALL_FILE_READ:

        {
            byte* buf      = (byte*)(info->esi);
            dword size     = (dword)(info->ecx);

            enableInterrupt();
            systemcall_mutex_lock(g_mutexFloppy);

            if (!g_fs->read(buf, size))
                {
                    info->eax = g_fs->getErrorNo();
                    systemcall_mutex_unlock(g_mutexFloppy);
                    break;
                }

            systemcall_mutex_unlock(g_mutexFloppy);
            disableInterrupt();
            info->eax = 0;
            break;
        }

    case SYSTEM_CALL_FILE_WRITE:

        {
            byte* buf      = (byte*)(info->esi);
            dword size     = (dword)(info->ecx);

            enableInterrupt();
            systemcall_mutex_lock(g_mutexFloppy);

            if (!g_fs->write(buf, size))
                {
                    info->eax = g_fs->getErrorNo();
                    systemcall_mutex_unlock(g_mutexFloppy);
                    break;
                }

            systemcall_mutex_unlock(g_mutexFloppy);
            disableInterrupt();
            info->eax = 0;
            break;
        }

    case SYSTEM_CALL_FILE_CREATE:

        {
            char* path = (char*)(info->esi);

            enableInterrupt();
            g_fdcdriver->motor(ON);
            g_fdcdriver->recalibrate();
            g_fdcdriver->recalibrate();
            g_fdcdriver->recalibrate();
            systemcall_mutex_lock(g_mutexFloppy);

            if (!g_fs->createFile(path))
                {
                    info->eax = g_fs->getErrorNo();
                    g_fdcdriver->motorAutoOff();
                    systemcall_mutex_unlock(g_mutexFloppy);
                    break;
                }

            systemcall_mutex_unlock(g_mutexFloppy);
            disableInterrupt();
            info->eax = 0;
            break;
        }

    case SYSTEM_CALL_FILE_CLOSE:

        enableInterrupt();
        systemcall_mutex_lock(g_mutexFloppy);
        g_fs->close();
        g_fdcdriver->motorAutoOff();
        systemcall_mutex_unlock(g_mutexFloppy);
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

        /* message has come. after your last peek or receive */
        if (g_currentThread->thread->flags & MEvent::MESSAGE)
            {
                break;
            }

        g_scheduler->WaitEvent(g_currentThread->thread, MEvent::MESSAGE);
        g_scheduler->SwitchToNext();

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
            bool isProcessChange = g_scheduler->SetNextThread();
            ThreadOperation::switchThread(isProcessChange, 17);
        }
        break;

    case SYSTEM_CALL_WAIT_FDC:

        {
            if (!g_fdcdriver->interrupted())
                {
                    g_scheduler->WaitEvent(g_currentThread->thread, MEvent::INTERRUPT_HIGH);
                    g_scheduler->SwitchToNext();

                    /* not reached */
                }
        }
        break;

    case SYSTEM_CALL_FDC_DISK_CHANGED:

        enableInterrupt();
        info->eax = g_fdcdriver->checkDiskChange();
        break;

    case SYSTEM_CALL_LOOKUP_MAIN_THREAD:

        info->eax = g_scheduler->LookupMainThread((char*)(info->esi));

        break;

    case SYSTEM_CALL_MEMORY_MAP_CREATE:

        {
            static dword sharedId = 0x9000;
            sharedId++;

            dword size = info->esi;

            while (Semaphore::down(&g_semaphore_shared));
            bool isOpen = SharedMemoryObject::open(sharedId, size);
            Semaphore::up(&g_semaphore_shared);

            if (!isOpen)
                {
                    info->eax = 0;
                    break;
                }
            info->eax = sharedId;
        }
        break;

    case SYSTEM_CALL_MEMORY_MAP_GET_SIZE:

        {
            dword id = info->esi;

            SharedMemoryObject* object = SharedMemoryObject::find(id);

            if (object == NULL)
                {
                    break;
                }

            info->eax = object->getSize();
        }
        break;

    case SYSTEM_CALL_MEMORY_MAP_MAP:

        {
            dword id      = info->esi;
            dword address = info->ecx;

            while (Semaphore::down(&g_semaphore_shared));
            bool isAttaced = SharedMemoryObject::attach(id, g_currentThread->process, address);
            Semaphore::up(&g_semaphore_shared);

            if (!isAttaced)
                {
                    info->eax = 1;
                    break;
                }
        }
        break;

    case SYSTEM_CALL_MEMORY_MAP_UNMAP:

        {
            dword id = info->esi;

            while (Semaphore::down(&g_semaphore_shared));
            bool isDetached = SharedMemoryObject::detach(id, g_currentThread->process);
            Semaphore::up(&g_semaphore_shared);

            info->eax = isDetached ? 0 : 1;
        }
        break;

    case SYSTEM_CALL_CD:
        {
            int result;
            char* path = (char*)(info->esi);
            enableInterrupt();
            g_fdcdriver->motor(ON);
            g_fdcdriver->recalibrate();
            g_fdcdriver->recalibrate();
            g_fdcdriver->recalibrate();

            systemcall_mutex_lock(g_mutexFloppy);

            result = g_fs->cd(path) ? 0 : 1;
            g_fdcdriver->motorAutoOff();

            systemcall_mutex_unlock(g_mutexFloppy);

            disableInterrupt();
            info->eax = result;
        }
        break;

    case SYSTEM_CALL_DIR_OPEN:

        {
            enableInterrupt();
            g_fdcdriver->motor(ON);
            g_fdcdriver->recalibrate();
            g_fdcdriver->recalibrate();
            g_fdcdriver->recalibrate();

            systemcall_mutex_lock(g_mutexFloppy);

            if (!g_fs->openDir())
                {
                    g_fdcdriver->motorAutoOff();
                    systemcall_mutex_unlock(g_mutexFloppy);
                    disableInterrupt();
                    info->eax = 1;
                    break;
                }

            systemcall_mutex_unlock(g_mutexFloppy);
            disableInterrupt();
            info->eax = 0;
        }
        break;

    case SYSTEM_CALL_DIR_CLOSE:

        {
            int result;
            enableInterrupt();
            systemcall_mutex_lock(g_mutexFloppy);

            result = g_fs->closeDir() ? 0 : 1;

            systemcall_mutex_unlock(g_mutexFloppy);
            g_fdcdriver->motorAutoOff();
            disableInterrupt();
            info->eax = result;
        }
        break;

    case SYSTEM_CALL_DIR_READ:

        {
            int result;
            char* name = (char*)info->esi;
            int* size = (int*)info->ecx;
            int* attr = (int*)info->edi;
            enableInterrupt();

            systemcall_mutex_lock(g_mutexFloppy);

            result = g_fs->readDir(name, size, attr) ? 0 : 1;

            systemcall_mutex_unlock(g_mutexFloppy);
            g_fdcdriver->motorAutoOff();
            disableInterrupt();
            info->eax = result;
        }
        break;

    case SYSTEM_CALL_PS_DUMP_SET:

        g_scheduler->SetDump();
        break;

    case SYSTEM_CALL_PS_DUMP_READ:

        {
            PsInfo* p = (PsInfo*)(info->esi);
            PsInfo* q = g_scheduler->ReadDump();

            if (q == NULL)
                {
                    info->eax = 1;
                    break;
                }

            *p = *q;
            delete q;
            info->eax = 0;
        }

        break;

    case SYSTEM_CALL_GET_TICK:

        info->eax = g_scheduler->GetTick();
        break;

    case SYSTEM_CALL_FILE_POSITION:

        info->eax = g_fs->position();
        break;

    case SYSTEM_CALL_FILE_SEEK:

        info->eax = g_fs->seek(info->esi, info->ecx);
        break;

    case SYSTEM_CALL_GET_KERNEL_VERSION:

        strncpy((char*)info->esi, version, info->ecx);
        info->eax = version_number;
        break;

    case SYSTEM_CALL_LOAD_PROCESS_IMAGE:

        {
            LoadProcessInfo* p = (LoadProcessInfo*)(info->esi);
            info->eax = Loader::Load(p->image, p->size, p->entrypoint, p->name, true, p->list);
            break;
        }

    case SYSTEM_CALL_CLEAR_SCREEN:

        g_console->clearScreen();
        info->eax = 0;
        break;

    case SYSTEM_CALL_PEEK:

        info->eax = g_messenger->peek(g_currentThread->thread
                                      , (MessageInfo*)(info->esi)
                                      , (int)(info->ecx)
                                      , (int)(info->edi)
                                      );

        break;

    case SYSTEM_CALL_TEST:

        {
            dword laddress = (dword)(info->esi);
            dword paddress;

            g_console->printf("conver=%s\n", g_page_manager->getPhysicalAddress(g_currentThread->process->getPageDirectory(), laddress, &paddress) ? "true" : "false");

            dword* p = (dword*)paddress;

            g_console->printf("laddress=%x paddress=%x", laddress, paddress);

            g_page_manager->setPageDirectory((dword)g_page_manager->getKernelDirectory());


            g_console->printf("pvalue=%x\n", *p);

            g_page_manager->setPageDirectory((dword)g_currentThread->process->getPageDirectory());
            break;
        }

    case SYSTEM_CALL_REMOVE_IRQ_RECEIVER:

        {
            int irq = (int)info->esi;

            /* out of range */
            if (irq > 15 || irq < 0)
                {
                    info->eax = 1;
                    break;
                }

            g_irqInfo[irq].hasReceiver = false;

            break;
        }

    case SYSTEM_CALL_SET_IRQ_RECEIVER:

        {
            int   irq     = (int)info->esi;

            /* out of range */
            if (irq > 15 || irq < 0)
                {
                    break;
                }

            g_irqInfo[irq].hasReceiver = true;
            g_irqInfo[irq].thread      = g_currentThread;

            break;
        }

    default:
        g_console->printf("syscall:default");
        break;
    }

    return;
}
