/*!
  \file  Loader.cpp
  \brief class Basic Process Loader

  Copyright (c) 2002-2004 Higepon and the individuals listed on the ChangeLog entries.
  All rights reserved.
  License=MIT/X Licnese

  \author  HigePon
  \version $Revision$
  \date   create:2004/06/15 update:$Date$
*/

#include "Loader.h"

/*----------------------------------------------------------------------
    Loader
----------------------------------------------------------------------*/
/*!
    \brief Read file

    \param  name file name
    \param  size size of been read file
    \author HigePon
    \date   create:2004/06/15 update:
*/
byte* Loader::ReadFile(const char* path, dword* size)
{
    dword readTimes;
    byte* buf;
    char name[128];

    strncpy(name, path, sizeof(name));

    /* only one process can use fd */
    while (Semaphore::down(&g_semaphore_fd));
    g_fdcdriver->motor(ON);
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();

    /* file open */
    if (!(g_fs->open(name, 1)))
    {
        g_fdcdriver->motorAutoOff();
        Semaphore::up(&g_semaphore_fd);
        return NULL;
    }

    /* get file size and allocate buffer */
    *size = g_fs->size();

    readTimes = (*size + 512 -1) / 512;
    buf       = (byte*)malloc(512 * readTimes);

    if (buf == NULL)
    {
        g_fdcdriver->motorAutoOff();
        Semaphore::up(&g_semaphore_fd);
        return NULL;
    }
    memset(buf, 0, 512 * readTimes);

    /* read */
    if (!g_fs->read(buf, *size))
    {
            free(buf);
            g_fdcdriver->motorAutoOff();
            Semaphore::up(&g_semaphore_fd);
            return NULL;
    }

    /* close */
    if (!g_fs->close())
    {
        g_fdcdriver->motorAutoOff();
        Semaphore::up(&g_semaphore_fd);
    }

    g_fdcdriver->motorAutoOff();
    Semaphore::up(&g_semaphore_fd);

    return buf;
}

int Loader::Load(byte* image, dword size, dword entrypoint, const char* name, bool isUser, CommandOption* list)
{
    /* shared ID */
    static dword sharedId = 0x2000;
    sharedId++;

    bool   isOpen;
    bool   isAttaced;

    /* attach Shared to this process */
    while (Semaphore::down(&g_semaphore_shared));
    isOpen    = SharedMemoryObject::open(sharedId, Loader::MAX_IMAGE_SIZE);
    isAttaced = SharedMemoryObject::attach(sharedId, g_currentThread->process, 0x80000000);
    Semaphore::up(&g_semaphore_shared);
    if (!isOpen || !isAttaced) return 4;

    /* create process */
    enter_kernel_lock_mode();
    Process* process = ProcessOperation::create(isUser ? ProcessOperation::USER_PROCESS : ProcessOperation::KERNEL_PROCESS, name);

    /* attach binary image to process */
    while (Semaphore::down(&g_semaphore_shared));
    isOpen    = SharedMemoryObject::open(sharedId, Loader::MAX_IMAGE_SIZE);
    isAttaced = SharedMemoryObject::attach(sharedId, process, Loader::ORG);
    Semaphore::up(&g_semaphore_shared);
    if (!isOpen || !isAttaced) return 5;

    memcpy((byte*)0x80000000, image, size);

    /* detach from this process */
    while (Semaphore::down(&g_semaphore_shared));
    SharedMemoryObject::detach(sharedId, g_currentThread->process);
    Semaphore::up(&g_semaphore_shared);

    /* set arguments */
    if (list != NULL)
    {
        char* p;
        CommandOption* option;
        List<char*>* target = process->getArguments();

        for (option = list->next; option; option = option->next)
        {
            p = new char[32];
            strncpy(p, option->str, 32);
            target->add(p);
        }
    }

    /* now process is loaded */
    Thread*  thread = ThreadOperation::create(process, entrypoint);
    g_scheduler->Join(thread);
    exit_kernel_lock_mode();

    return 0;
}

int Loader::Load(const char* path, const char* name, bool isUser, CommandOption* list)
{
    int result;
    byte* image;
    dword size;

    image = Loader::ReadFile(path, &size);
    if (image == NULL || size == 0)
    {
        return 1;
    }

    result = Loader::Load(image, size, Loader::ORG, name, isUser, list);

    if (result != 0)
    {
        free(image);
        return result;
    }

    free(image);
    return 0;
}
