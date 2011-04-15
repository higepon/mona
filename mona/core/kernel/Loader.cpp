/*!
  \file  Loader.cpp
  \brief class Basic Process Loader

  Copyright (c) 2002-2004 Higepon and the individuals listed on the ChangeLog entries.
  All rights reserved.
  License=MIT/X License

  \author  Higepon
  \version $Revision$
  \date   create:2004/06/15 update:$Date$
*/

#include "Loader.h"

/*----------------------------------------------------------------------
    Loader
----------------------------------------------------------------------*/
intptr_t Loader::Load(uint8_t* image, uint32_t size, uint32_t entrypoint, const char* name, bool isUser, CommandOption* list, uint32_t observer, uint32_t& tid)
{
    ASSERT(size < MAX_IMAGE_SIZE);

    int memSize = ((size + PageManager::ARCH_PAGE_SIZE - 1) / PageManager::ARCH_PAGE_SIZE) * PageManager::ARCH_PAGE_SIZE; 

    SharedMemoryObject* shm = SharedMemoryObject::create(memSize);
    if (shm == NULL) {
        return M_BAD_MEMORY_MAP_ID;
    }

    intptr_t ret = shm->attach(g_page_manager, g_currentThread->process, 0x80000000, true);
    if (ret != M_OK) {
        return ret;
    }

    /* create process */
    enter_kernel_lock_mode();
    Process* process = ProcessOperation::create(isUser ? ProcessOperation::USER_PROCESS : ProcessOperation::KERNEL_PROCESS, name);

    ret = shm->attach(g_page_manager, process, Loader::ORG, true);
    if (ret != M_OK) {
        return ret;
    }
    memcpy((uint8_t*)0x80000000, image, size);
    /* detach from this process */
    shm->detach(g_page_manager, g_currentThread->process);
    SharedMemoryObject::destroy(shm);

    setupArguments(process, list);

    Thread*  thread = ThreadOperation::create(process, entrypoint, observer);

    // Ugly, workaround.
    // We want to mark process_server as observer.
    thread->observers.add(g_currentThread->thread->id);
    tid = thread->id;
    g_scheduler->Join(thread);
    exit_kernel_lock_mode();
    return M_OK;
}

intptr_t Loader::LoadFromMemoryMap(uint32_t handle, uint32_t entrypoint, const char* name, CommandOption* list, uint32_t observer)
{
    SharedMemoryObject* shm = SharedMemoryObject::find(handle);
    if (shm == NULL) {
        return M_BAD_MEMORY_MAP_ID;
    }

    enter_kernel_lock_mode();
    Process* process = ProcessOperation::create(ProcessOperation::USER_PROCESS, name);

    intptr_t ret = shm->attach(g_page_manager, process, Loader::ORG, true);
    if (ret != M_OK) {
        return ret;
    }

    setupArguments(process, list);

    /* now process is loaded */
    Thread*  thread = ThreadOperation::create(process, entrypoint, observer);
    // Ugly, workaround.
    // We want to mark process_server as observer.
    thread->observers.add(g_currentThread->thread->id);
    g_scheduler->Join(thread);
    exit_kernel_lock_mode();
    return M_OK;
}

void Loader::setupArguments(Process* process, CommandOption* list)
{
    if (list == NULL) {
        return ;
    }
    char* p;
    CommandOption* option;
    List<char*>* target = process->getArguments();

    for (option = list->next; option; option = option->next)
    {
        p = new char[MAX_PROCESS_ARGUMENT_LENGTH];
        strncpy(p, option->str, MAX_PROCESS_ARGUMENT_LENGTH);
        bool terminated = false;
        for (int i = 0; i < MAX_PROCESS_ARGUMENT_LENGTH; i++)
        {
            if (option->str[i] == '\0')
            {
                terminated = true;
                break;
            }
        }
        if (!terminated)
        {
            p[MAX_PROCESS_ARGUMENT_LENGTH - 1] = '\0';
        }
        target->add(p);
    }
}
