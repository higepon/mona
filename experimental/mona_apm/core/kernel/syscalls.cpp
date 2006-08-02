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

void syscall_entrance()
{
    ScreenInfo* screenInfo;
    ArchThreadInfo* info = g_currentThread->archinfo;

    /* result normal */
    info->eax = 0;

    switch(info->ebx)
    {
    case SYSTEM_CALL_PRINT:

        //enableInterrupt();
        g_console->printf("%s", (char*)(info->esi));
        break;

    case SYSTEM_CALL_SET_TIMER:

        info->eax = g_scheduler->SetTimer(g_currentThread->thread, info->esi);
        break;

    case SYSTEM_CALL_KILL_TIMER:

        info->eax = g_scheduler->KillTimer(info->esi, g_currentThread->thread);
        break;

    case SYSTEM_CALL_MTHREAD_SLEEP:

        g_scheduler->Sleep(g_currentThread->thread, info->esi);
        g_scheduler->SwitchToNext();
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

        /* not reached */

        break;

    case SYSTEM_CALL_RECEIVE:

        info->eax = g_messenger->receive(g_currentThread->thread, (MessageInfo*)(info->esi));
        break;

    case SYSTEM_CALL_EXIST_MESSAGE:
    {
        bool existMessage = !(g_currentThread->thread->messageList->isEmpty());
        info->eax = existMessage ? 1 : 0;
        break;
    }

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

        break;
    }

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
        break;
    }

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
        break;
    }


    case SYSTEM_CALL_LOOKUP:
        info->eax = g_scheduler->Lookup((char*)(info->esi));
        break;

    case SYSTEM_CALL_GET_VRAM_INFO:
        screenInfo = (ScreenInfo*)(info->esi);
        screenInfo->vram = (dword)(g_vesaDetail->physBasePtr);
        screenInfo->bpp  = (dword)(g_vesaDetail->bitsPerPixel);
        screenInfo->x    = (dword)(g_vesaDetail->xResolution);
        screenInfo->y    = (dword)(g_vesaDetail->yResolution);
        break;

    case SYSTEM_CALL_LOAD_PROCESS:

        g_console->printf("this systemcall not supported %s:%d\n", __FILE__, __LINE__);
        break;

    case SYSTEM_CALL_SET_CURSOR:

        g_console->setCursor((int)(info->esi), (int)(info->ecx));
        break;

    case SYSTEM_CALL_GET_CURSOR:

        g_console->getCursor((int*)(info->esi), (int*)(info->ecx));
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
        char* buf = (char*)(info->esi);
        int index = (int)(info->ecx);

        if (index - 1 > list->size())
        {
            info->eax = 1;
            break;
        }

        strcpy(buf, list->get(index));
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
        KDate* date = (KDate*)(info->esi);
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

	if (info->esi == NULL)
	{
	    info->eax = g_scheduler->LookupMainThread(g_currentThread->process);
	}
	else
	{
	    info->eax = g_scheduler->LookupMainThread((char*)(info->esi));
	}
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
        break;
     }


    case SYSTEM_CALL_MEMORY_MAP_GET_SIZE:
    {
        dword id = info->esi;

        SharedMemoryObject* object = SharedMemoryObject::find(id);

        if (object == NULL)
        {
            break;
        }

        info->eax = object->getSize();
        break;
    }


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
        break;
    }


    case SYSTEM_CALL_MEMORY_MAP_UNMAP:
    {
        dword id = info->esi;

        while (Semaphore::down(&g_semaphore_shared));
        bool isDetached = SharedMemoryObject::detach(id, g_currentThread->process);
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
        g_irqInfo[irq].maskInterrupt = false;

        break;
    }

    case SYSTEM_CALL_SET_IRQ_RECEIVER:
    {
        int irq  = (int)info->esi;
        bool maskInterrupt = info->ecx != 0;

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
        int irq  = (int)info->esi;

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
        dword address = info->esi;
        dword size    = info->ecx;

        g_page_manager->returnPages(g_currentThread->process->getPageDirectory(), address, size);
        break;
    }

    case SYSTEM_CALL_GET_MEMORY_INFO:
    {
        MemoryInfo* i = (MemoryInfo*)(info->esi);

        i->totalMemoryL = g_total_system_memory;
        g_page_manager->getPagePoolInfo(&(i->freePageNum), &(i->totalPageNum), &(i->pageSize));
        break;
    }

    case SYSTEM_CALL_LOG_PRINT:
    {
        logprintf("%s", (const char*)(info->esi));
        break;
    }

    case SYSTEM_CALL_ALLOCATE_DMA_MEMORY:

        info->eax = (dword)g_page_manager->allocateDMAMemory(g_currentThread->process->getPageDirectory(), true);
        break;

    case SYSTEM_CALL_DEALLOCATE_DMA_MEMORY:

        g_page_manager->deallocateDMAMemory(g_currentThread->process->getPageDirectory(), info->esi);
        break;

    case SYSTEM_CALL_CHANGE_BASE_PRIORITY:
        g_scheduler->ChangeBasePriority(g_currentThread->thread, info->esi);

    case SYSTEM_CALL_SET_DLL_SEGMENT_WRITABLE:
	g_currentThread->process->getDllSegment()->setWritable(true);
	break;

    case SYSTEM_CALL_SET_DLL_SEGMENT_NOTSHARED:
	g_dllSharedObject->setPageFlag(info->esi, SharedMemoryObject::FLAG_NOT_SHARED);
	break;

    case SYSTEM_CALL_APM_SET_POWER_STATE:
        info->eax = (dword)apm_set_power_state((word)info->esi, (word)info->ecx);
        break;

    case SYSTEM_CALL_APM_GET_POWER_STATE:
	info->eax = (dword)apm_get_power_state((word)info->esi);
	break;

    default:
        g_console->printf("syscall:default");
        break;
    }

    return;
}
