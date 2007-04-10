/*!
  \file  syscalls.cpp
  \brief syscalls

  Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
  All rights reserved.
  License=MIT/X License

  \author  HigePon
  \version $Revision$
  \date   create:2003/03/22 update:$Date$
*/

#include "syscalls.h"
#include "global.h"
#include "io.h"
#include "Loader.h"
#include "RTC.h"
#include "apm.h"
#include "shutdown.h"

extern const char* version;
extern uint32_t version_number;

uint32_t systemcall_mutex_create()
{
    KMutex* mutex = new KMutex();
    return g_id->allocateID(mutex);
}

uint32_t systemcall_mutex_lock(uint32_t id)
{
    uint32_t result;
    SYSCALL_1(SYSTEM_CALL_MUTEX_LOCK, result, id);
    return result;
}

uint32_t systemcall_mutex_unlock(uint32_t id)
{
   int result;
    SYSCALL_1(SYSTEM_CALL_MUTEX_UNLOCK, result, id);
    return result;
}

// don't call without systemcall
// this has context change
// use systemcall_mutex_lock()
static uint32_t systemcall_mutex_lock2(uint32_t id)
{
    KObject* object = g_id->get(id, g_currentThread->thread);
    if (object == NULL)
    {
        return g_id->getLastError();
    }
    else if (object->getType() != KObject::KMUTEX)
    {
        return (uint32_t)-10;
    }
    return ((KMutex*)object)->lock(g_currentThread->thread);
}

// don't call without systemcall
// this has context change
// use systemcall_mutex_unlock()
static uint32_t systemcall_mutex_unlock2(uint32_t id)
{
    KObject* object = g_id->get(id, g_currentThread->thread);

    if (object == NULL)
    {
        return g_id->getLastError();
    }
    else if (object->getType() != KObject::KMUTEX)
    {
        return (uint32_t)-10;
    }

    return ((KMutex*)object)->unlock();
}

void syscall_entrance()
{
    ScreenInfo* screenInfo;
    ArchThreadInfo* info = g_currentThread->archinfo;

    /* result normal */
    info->eax = 0;

#define SYSTEM_CALL_ARG_1 info->esi
#define SYSTEM_CALL_ARG_2 info->ecx
#define SYSTEM_CALL_ARG_3 info->edi

    switch(info->ebx)
    {
    case SYSTEM_CALL_PRINT:

        //enableInterrupt();
        g_console->printf("%s", (char*)(SYSTEM_CALL_ARG_1));
        break;

    case SYSTEM_CALL_SET_TIMER:

        info->eax = g_scheduler->SetTimer(g_currentThread->thread, SYSTEM_CALL_ARG_1);
        break;

    case SYSTEM_CALL_KILL_TIMER:

        info->eax = g_scheduler->KillTimer(SYSTEM_CALL_ARG_1, g_currentThread->thread);
        break;

    case SYSTEM_CALL_MTHREAD_SLEEP:

        g_scheduler->Sleep(g_currentThread->thread, SYSTEM_CALL_ARG_1);
        g_scheduler->SwitchToNext();
        break;

    case SYSTEM_CALL_KILL:

        ThreadOperation::kill();
        break;

    case SYSTEM_CALL_KILL_THREAD:
    {
        uint32_t tid = SYSTEM_CALL_ARG_1;
        info->eax = ThreadOperation::kill(tid);
        break;
    }

    case SYSTEM_CALL_SEND:

        info->eax = g_messenger->send((uint32_t)(SYSTEM_CALL_ARG_1), (MessageInfo*)(SYSTEM_CALL_ARG_2));
        g_scheduler->SwitchToNext();

        /* not reached */

        break;

    case SYSTEM_CALL_RECEIVE:

        info->eax = g_messenger->receive(g_currentThread->thread, (MessageInfo*)(SYSTEM_CALL_ARG_1));
        break;

    case SYSTEM_CALL_EXIST_MESSAGE:
    {
        bool existMessage = !(g_currentThread->thread->messageList->isEmpty());
        info->eax = existMessage ? 1 : 0;
        break;
    }


    case SYSTEM_CALL_MTHREAD_CREATE:
    {
        uint32_t arg = SYSTEM_CALL_ARG_2;
        Thread* thread = ThreadOperation::create(g_currentThread->process, SYSTEM_CALL_ARG_1);
        thread->tinfo->archinfo->ecx = arg;
        g_scheduler->Join(thread);
        info->eax = g_id->allocateID(thread);
        break;
    }
    case SYSTEM_CALL_MTHREAD_KILL:
    {
        KObject* object = g_id->get(SYSTEM_CALL_ARG_1, g_currentThread->thread);

        if (object == NULL)
        {
            info->eax = g_id->getLastError();
        }
        else if (object->getType() != KObject::THREAD)
        {
            info->eax = (uint32_t)-10;
        }
        else
        {
            Thread* t = (Thread*)object;
            info->eax = ThreadOperation::kill(t->id);
        }

        break;
    }

    case SYSTEM_CALL_MUTEX_CREATE:
        if (SYSTEM_CALL_ARG_1 != 0) {
            KObject* object = g_id->get(SYSTEM_CALL_ARG_1, g_currentThread->thread);
            if (object == NULL)
            {            info->eax = g_id->getLastError();
            }
            else if (object->getType() != KObject::KMUTEX)
            {
                info->eax = (uint32_t)-10;
            }
            else
            {
                KMutex* mutex = (KMutex*)object;
                mutex->addRef();
                info->eax = SYSTEM_CALL_ARG_1;
            }
        } else {
            info->eax = systemcall_mutex_create();
        }
        break;

    case SYSTEM_CALL_MUTEX_LOCK:

        info->eax = systemcall_mutex_lock2(SYSTEM_CALL_ARG_1);
        break;

    case SYSTEM_CALL_MUTEX_TRYLOCK:
    {
        KObject* object = g_id->get(SYSTEM_CALL_ARG_1, g_currentThread->thread);

        if (object == NULL)
        {
            info->eax = g_id->getLastError();
        }
        else if (object->getType() != KObject::KMUTEX)
        {
            info->eax = (uint32_t)-10;
        }
        else
        {

            info->eax = ((KMutex*)object)->tryLock(g_currentThread->thread);
        }
        break;
    }

    case SYSTEM_CALL_MUTEX_UNLOCK:

        info->eax = systemcall_mutex_unlock2(SYSTEM_CALL_ARG_1);
        break;

    case SYSTEM_CALL_MUTEX_DESTROY:

    {
        KObject* object = g_id->get(SYSTEM_CALL_ARG_1, g_currentThread->thread);

        if (object == NULL)
        {
            info->eax = g_id->getLastError();
        }
        else if (object->getType() != KObject::KMUTEX)
        {
            info->eax = (uint32_t)-10;
        }
        else
        {
            KMutex* mutex = (KMutex*)object;
            mutex->releaseRef();
            info->eax = 0;
        }
        break;
    }


    case SYSTEM_CALL_LOOKUP:
        info->eax = g_scheduler->Lookup((char*)(SYSTEM_CALL_ARG_1));
        break;

    case SYSTEM_CALL_GET_VRAM_INFO:
        screenInfo = (ScreenInfo*)(SYSTEM_CALL_ARG_1);
        screenInfo->vram = (uint32_t)(g_vesaDetail->physBasePtr);
        screenInfo->bpp  = (uint32_t)(g_vesaDetail->bitsPerPixel);
        screenInfo->x    = (uint32_t)(g_vesaDetail->xResolution);
        screenInfo->y    = (uint32_t)(g_vesaDetail->yResolution);
        break;

    case SYSTEM_CALL_LOAD_PROCESS:

        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_SET_CURSOR:

        g_console->setCursor((int)(SYSTEM_CALL_ARG_1), (int)(SYSTEM_CALL_ARG_2));
        break;

    case SYSTEM_CALL_GET_CURSOR:

        g_console->getCursor((int*)(SYSTEM_CALL_ARG_1), (int*)(SYSTEM_CALL_ARG_2));
        break;

    case SYSTEM_CALL_FDC_OPEN:

        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_FDC_CLOSE:

        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_FDC_READ:
        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_FDC_WRITE:
        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_FILE_OPEN:
        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_FILE_READ:
        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_FILE_WRITE:
        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_FILE_CREATE:
        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_FILE_CLOSE:
        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
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
        char* buf = (char*)(SYSTEM_CALL_ARG_1);
        int index = (int)(SYSTEM_CALL_ARG_2);

        if (index - 1 > list->size())
        {
            info->eax = 1;
            break;
        }

        strncpy(buf, list->get(index), MAX_PROCESS_ARGUMENT_LENGTH);
        info->eax = 0;
        break;
    }

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
        KDate* date = (KDate*)(SYSTEM_CALL_ARG_1);
        RTC::getDate(date);
        info->eax = 0;
        break;
    }


    case SYSTEM_CALL_GET_IO:

        info->eflags = info->eflags |  0x3000;
        info->eax = 0;
        {
            bool isProcessChange = g_scheduler->SetNextThread();
            ThreadOperation::switchThread(isProcessChange, 17);
        }
        break;

    case SYSTEM_CALL_FDC_DISK_CHANGED:
        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_LOOKUP_MAIN_THREAD:

    if (SYSTEM_CALL_ARG_1 == NULL)
    {
        info->eax = g_scheduler->LookupMainThread(g_currentThread->process);
    }
    else
    {
        info->eax = g_scheduler->LookupMainThread((char*)(SYSTEM_CALL_ARG_1));
    }
        break;

    case SYSTEM_CALL_MEMORY_MAP_CREATE:
    {
        static uint32_t sharedId = 0x9000;
        sharedId++;

        uint32_t size = SYSTEM_CALL_ARG_1;

        while (Semaphore::down(&g_semaphore_shared));
        bool isOpen = SharedMemoryObject::open(sharedId, size);
        Semaphore::up(&g_semaphore_shared);

        if (!isOpen)
        {
            g_console->printf("kernel:::");
            info->eax = 0;
            break;
        }
            g_console->printf("kernel[[[[");
        info->eax = sharedId;
        break;
     }


    case SYSTEM_CALL_MEMORY_MAP_GET_SIZE:
    {
        uint32_t id = SYSTEM_CALL_ARG_1;

        SharedMemoryObject* object = SharedMemoryObject::find(id);

        if (object == NULL)
        {
            g_console->printf("kernel***");
            break;
        }

        info->eax = object->getSize();
        break;
    }


    case SYSTEM_CALL_MEMORY_MAP_MAP:
    {
        uint32_t id      = SYSTEM_CALL_ARG_1;
        uint32_t address = SYSTEM_CALL_ARG_2;

        while (Semaphore::down(&g_semaphore_shared));
        bool isAttaced = SharedMemoryObject::attach(id, g_currentThread->process, address);
        Semaphore::up(&g_semaphore_shared);

        if (!isAttaced)
        {
            info->eax = 1;
            break;
        }
        break;
    }


    case SYSTEM_CALL_MEMORY_MAP_UNMAP:
    {
        uint32_t id = SYSTEM_CALL_ARG_1;

        while (Semaphore::down(&g_semaphore_shared));
        bool isDetached = SharedMemoryObject::detach(id, g_currentThread->process);
            g_console->printf("unmap***");
        Semaphore::up(&g_semaphore_shared);

        info->eax = isDetached ? 0 : 1;
        break;
    }

    case SYSTEM_CALL_CD:
        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_DIR_OPEN:
        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_DIR_CLOSE:
        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_DIR_READ:
        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_PS_DUMP_SET:

        g_scheduler->SetDump();
        break;

    case SYSTEM_CALL_PS_DUMP_READ:
    {
        PsInfo* p = (PsInfo*)(SYSTEM_CALL_ARG_1);
        PsInfo* q = g_scheduler->ReadDump();

        if (q == NULL)
        {
            info->eax = 1;
            break;
        }

        *p = *q;
        delete q;
        info->eax = 0;
        break;
    }

    case SYSTEM_CALL_GET_TICK:

        info->eax = g_scheduler->GetTick();
        break;

    case SYSTEM_CALL_FILE_POSITION:

        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_FILE_SEEK:

        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_GET_KERNEL_VERSION:

        strncpy((char*)SYSTEM_CALL_ARG_1, version, SYSTEM_CALL_ARG_2);
        info->eax = version_number;
        break;

    case SYSTEM_CALL_LOAD_PROCESS_IMAGE:
    {
        LoadProcessInfo* p = (LoadProcessInfo*)(SYSTEM_CALL_ARG_1);
        info->eax = Loader::Load(p->image, p->size, p->entrypoint, p->name, true, p->list);
        break;
    }

    case SYSTEM_CALL_CLEAR_SCREEN:

        g_console->clearScreen();
        info->eax = 0;
        break;

    case SYSTEM_CALL_PEEK:

        info->eax = g_messenger->peek(g_currentThread->thread
                                      , (MessageInfo*)(SYSTEM_CALL_ARG_1)
                                      , (int)(SYSTEM_CALL_ARG_2)
                                      , (int)(SYSTEM_CALL_ARG_3)
                                      );

        break;

    case SYSTEM_CALL_REMOVE_IRQ_RECEIVER:
    {
        int irq = (int)SYSTEM_CALL_ARG_1;

        /* out of range */
        if (irq > 15 || irq < 0)
        {
            info->eax = 1;
            break;
        }

        g_irqInfo[irq].hasReceiver = false;
        g_irqInfo[irq].maskInterrupt = false;

        break;
    }

    case SYSTEM_CALL_SET_IRQ_RECEIVER:
    {
        int irq  = (int)SYSTEM_CALL_ARG_1;
        bool maskInterrupt = SYSTEM_CALL_ARG_2 != 0;

        /* out of range */
        if (irq > 15 || irq < 0)
        {
            break;
        }

        g_irqInfo[irq].hasReceiver = true;
        g_irqInfo[irq].thread      = g_currentThread;
        g_irqInfo[irq].maskInterrupt = maskInterrupt;

        break;
    }
    case SYSTEM_CALL_HAS_IRQ_RECEIVER:
    {
        int irq  = (int)SYSTEM_CALL_ARG_1;

        /* out of range */
        if (irq > 15 || irq < 0)
        {
            break;
        }
        info->eax = g_irqInfo[irq].hasReceiver ? 1 : 0;
        break;
    }

    case SYSTEM_CALL_FREE_PAGES:
    {
        uint32_t address = SYSTEM_CALL_ARG_1;
        uint32_t size    = SYSTEM_CALL_ARG_2;

        g_page_manager->returnPages(g_currentThread->process->getPageDirectory(), address, size);
        break;
    }

    case SYSTEM_CALL_GET_MEMORY_INFO:
    {
        MemoryInfo* i = (MemoryInfo*)(SYSTEM_CALL_ARG_1);

        i->totalMemoryL = g_total_system_memory;
        g_page_manager->getPagePoolInfo(&(i->freePageNum), &(i->totalPageNum), &(i->pageSize));
        break;
    }

    case SYSTEM_CALL_LOG_PRINT:
    {
        logprintf("%s", (const char*)(SYSTEM_CALL_ARG_1));
        break;
    }

    case SYSTEM_CALL_ALLOCATE_DMA_MEMORY:
    {
        uint32_t size = SYSTEM_CALL_ARG_1;
        info->eax = (uint32_t)g_page_manager->allocateDMAMemory(g_currentThread->process->getPageDirectory(), size, true);
        break;
    }
    case SYSTEM_CALL_DEALLOCATE_DMA_MEMORY:

        g_page_manager->deallocateDMAMemory(g_currentThread->process->getPageDirectory(), SYSTEM_CALL_ARG_1, SYSTEM_CALL_ARG_2);
        break;

    case SYSTEM_CALL_CHANGE_BASE_PRIORITY:
        g_scheduler->ChangeBasePriority(g_currentThread->thread, SYSTEM_CALL_ARG_1);

    case SYSTEM_CALL_SET_DLL_SEGMENT_WRITABLE:
        g_currentThread->process->getDllSegment()->setWritable(true);
    break;

    case SYSTEM_CALL_SET_DLL_SEGMENT_NOTSHARED:
        g_dllSharedObject->setPageFlag(SYSTEM_CALL_ARG_1, SharedMemoryObject::FLAG_NOT_SHARED);
    break;

    case SYSTEM_CALL_APM_BIOS:
        info->eax = (uint32_t)apm_bios((uint16_t)SYSTEM_CALL_ARG_1, (apm_bios_regs*)SYSTEM_CALL_ARG_2);
    break;

    case SYSTEM_CALL_SHUTDOWN:
        info->eax = shutdown(SYSTEM_CALL_ARG_1, SYSTEM_CALL_ARG_2);
    break;

    default:
        g_console->printf("syscall:default");
        break;
    }

    return;
}
