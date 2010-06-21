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
    /* shared ID */
    static uint32_t sharedId = 0x2000;
    sharedId++;

    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);

    bool   isOpen;
    bool   isAttaced;

    /* attach Shared to this process */
    systemcall_mutex_lock(g_mutexShared);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    isOpen    = SharedMemoryObject::open(sharedId, Loader::MAX_IMAGE_SIZE);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    isAttaced = SharedMemoryObject::attach(sharedId, g_currentThread->process, 0x80000000);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);

    systemcall_mutex_unlock(g_mutexShared);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    if (!isOpen || !isAttaced) return 4;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    /* create process */
    enter_kernel_lock_mode();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    Process* process = ProcessOperation::create(isUser ? ProcessOperation::USER_PROCESS : ProcessOperation::KERNEL_PROCESS, name);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);

    /* attach binary image to process */
    systemcall_mutex_lock(g_mutexShared);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);

    isOpen    = SharedMemoryObject::open(sharedId, Loader::MAX_IMAGE_SIZE);
    isAttaced = SharedMemoryObject::attach(sharedId, process, Loader::ORG);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    systemcall_mutex_unlock(g_mutexShared);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    if (!isOpen || !isAttaced) return 5;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    memcpy((uint8_t*)0x80000000, image, size);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);

    /* detach from this process */
    systemcall_mutex_lock(g_mutexShared);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    SharedMemoryObject::detach(sharedId, g_currentThread->process);
    systemcall_mutex_unlock(g_mutexShared);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    /* set arguments */
    if (list != NULL)
    {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
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
    logprintf("%s %s:%d entrypoint=%x\n", __func__, __FILE__, __LINE__, entrypoint);
    /* now process is loaded */
    Thread*  thread = ThreadOperation::create(process, entrypoint);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    g_scheduler->Join(thread);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    exit_kernel_lock_mode();

    return 0;
}

