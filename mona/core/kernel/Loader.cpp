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
int Loader::Load(uint8_t* image, uint32_t size, uint32_t entrypoint, const char* name, bool isUser, CommandOption* list)
{
    ASSERT(size < MAX_IMAGE_SIZE);
    /* attach Shared to this process */
    systemcall_mutex_lock(g_mutexShared);

    SharedMemoryObject* shm = SharedMemoryObject::create(Loader::MAX_IMAGE_SIZE);
    if (shm == NULL) {
        systemcall_mutex_unlock(g_mutexShared);
        return 4;
    }

    if (shm->attach(g_page_manager, g_currentThread->process, 0x80000000, false) != M_OK) {
        systemcall_mutex_unlock(g_mutexShared);
        return 4;
    }

    /* create process */
    enter_kernel_lock_mode();
    Process* process = ProcessOperation::create(isUser ? ProcessOperation::USER_PROCESS : ProcessOperation::KERNEL_PROCESS, name);

    /* attach binary image to process */
    systemcall_mutex_lock(g_mutexShared);

    if (shm->attach(g_page_manager, process, Loader::ORG, false) != M_OK) {
        systemcall_mutex_unlock(g_mutexShared);
        return 5;
    }

    systemcall_mutex_unlock(g_mutexShared);

    memcpy((uint8_t*)0x80000000, image, size);

    /* detach from this process */
    systemcall_mutex_lock(g_mutexShared);

    shm->detach(g_page_manager, g_currentThread->process);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    SharedMemoryObject::destroy(shm);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    systemcall_mutex_unlock(g_mutexShared);

    /* set arguments */
    if (list != NULL)
    {
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
    /* now process is loaded */
    Thread*  thread = ThreadOperation::create(process, entrypoint);
    g_scheduler->Join(thread);
    exit_kernel_lock_mode();

    return 0;
}
