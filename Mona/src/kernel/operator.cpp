/*!
    \file   operator.cpp
    \brief  define operator new & delete

    define operator new & delete

    Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2002/08/08 update:$Date$
*/
#include<MemoryManager.h>
#include<operator.h>
#include<io.h>
#include<syscalls.h>
#include<sys/types.h>
#include<global.h>

#ifndef BUILD_ON_LINUX
void* operator new(size_t size)
{
    enter_kernel_lock_mode();
    void* p = km.allocate(size);
    exit_kernel_lock_mode();
    return p;
}

void operator delete(void* address)
{
    enter_kernel_lock_mode();
    km.free(address);
    exit_kernel_lock_mode();
    return;
}

void* operator new[](size_t size)
{
    enter_kernel_lock_mode();
    void* p = km.allocate(size);
    exit_kernel_lock_mode();
    return p;
}

void operator delete[](void* address)
{
    enter_kernel_lock_mode();
    km.free(address);
    exit_kernel_lock_mode();
    return;
}

#else

void __builtin_delete(void* address)
{
    enter_kernel_lock_mode();
    km.free(address);
    exit_kernel_lock_mode();
    return;
    return;
}

void* __builtin_new(unsigned long size)
{
    enter_kernel_lock_mode();
    void* p = km.allocate(size);
    exit_kernel_lock_mode();
    return p;
}

void* __builtin_vec_new(unsigned long size) {

    return __builtin_new(size);
}

void __builtin_vec_delete(void* address) {

    __builtin_delete(address);
    return;
}

#endif

void* malloc(unsigned long size)
{
    enter_kernel_lock_mode();
    void* p = km.allocate(size);
    exit_kernel_lock_mode();
    return p;
}

void free(void * address)
{
    enter_kernel_lock_mode();
    km.free(address);
    exit_kernel_lock_mode();
    return;
}



