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

#include <stdafx.h>

extern const char* version;
extern dword version_number;

void syscall_entrance() {

#if 0 ///
    ScreenInfo* screenInfo;
    ArchThreadInfo* info = g_currentThread->archinfo;

    info->eax = 0;

    switch(info->ebx) {

    case SYSTEM_CALL_KILL:

        ThreadOperation::kill();
        break;

    case SYSTEM_CALL_KILL_THREAD:

        {
            dword tid = info->esi;
            info->eax = ThreadOperation::kill(tid);
            break;
        }

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

        {
            KMutex* mutex = new KMutex(g_currentThread->process);
            info->eax = g_id->allocateID(mutex);
        }
        break;

    case SYSTEM_CALL_MUTEX_LOCK:

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
                info->eax = ((KMutex*)object)->lock(g_currentThread->thread);
            }
        }
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
                info->eax = ((KMutex*)object)->unlock();
            }
        }
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

    case SYSTEM_CALL_FDC_OPEN:

        enableInterrupt();
            while (Semaphore::down(&g_semaphore_fd)) {g_console->printf("wait fdc open");};

        g_fdcdriver->motor(ON);
        g_fdcdriver->recalibrate();
        g_fdcdriver->recalibrate();
        g_fdcdriver->recalibrate();

        Semaphore::up(&g_semaphore_fd);
        info->eax = 0;
        break;

    case SYSTEM_CALL_FDC_CLOSE:

        enableInterrupt();
            while (Semaphore::down(&g_semaphore_fd)) {g_console->printf("wait fdc close");};

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

            while (Semaphore::down(&g_semaphore_fd)) {g_console->printf("wait fdc read");};
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

            while (Semaphore::down(&g_semaphore_fd)) {g_console->printf("wait fdc write");};
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

            while (Semaphore::down(&g_semaphore_fd)) {g_console->printf("wait file open");};

            if (!g_fs->open(path, mode))
            {
                info->eax = g_fs->getErrorNo();
                g_fdcdriver->motorAutoOff();
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
            while (Semaphore::down(&g_semaphore_fd)) {g_console->printf("wait file read");};

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

    case SYSTEM_CALL_FILE_WRITE:

        {
            byte* buf      = (byte*)(info->esi);
            dword size     = (dword)(info->ecx);

            enableInterrupt();
            while (Semaphore::down(&g_semaphore_fd)) {g_console->printf("wait file write");};

            if (!g_fs->write(buf, size))
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

    case SYSTEM_CALL_FILE_CREATE:

        {
            char* path = (char*)(info->esi);

            enableInterrupt();
            g_fdcdriver->motor(ON);
            g_fdcdriver->recalibrate();
            g_fdcdriver->recalibrate();
            g_fdcdriver->recalibrate();
            while (Semaphore::down(&g_semaphore_fd)) {g_console->printf("wait file create");};

            if (!g_fs->createFile(path))
            {
                info->eax = g_fs->getErrorNo();
                g_fdcdriver->motorAutoOff();
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
        while (Semaphore::down(&g_semaphore_fd)) {g_console->printf("wait file close");};
        g_fs->close();
        g_fdcdriver->motorAutoOff();
        Semaphore::up(&g_semaphore_fd);
        break;

    case SYSTEM_CALL_GET_PID:

        info->eax = g_currentThread->process->getPid();
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
           while (Semaphore::down(&g_semaphore_fd));

           result = g_fs->cd(path) ? 0 : 1;
           g_fdcdriver->motorAutoOff();
           Semaphore::up(&g_semaphore_fd);
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
           while (Semaphore::down(&g_semaphore_fd));

           if (!g_fs->openDir())
           {
               g_fdcdriver->motorAutoOff();
               Semaphore::up(&g_semaphore_fd);
               disableInterrupt();
               info->eax = 1;
               break;
           }

           Semaphore::up(&g_semaphore_fd);
           disableInterrupt();
           info->eax = 0;
       }
       break;

    case SYSTEM_CALL_DIR_CLOSE:

       {
           int result;
           enableInterrupt();
           while (Semaphore::down(&g_semaphore_fd));

           result = g_fs->closeDir() ? 0 : 1;
           Semaphore::up(&g_semaphore_fd);
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
           while (Semaphore::down(&g_semaphore_fd));

           result = g_fs->readDir(name, size, attr) ? 0 : 1;
           Semaphore::up(&g_semaphore_fd);
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

    default:
        g_console->printf("syscall:default");
        break;
    }

    return;
#endif
}
