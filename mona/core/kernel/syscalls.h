/*!
    \file  syscalls.h
    \brief syscalls

    Copyright (c) 2003 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2003/03/22 update:$Date$
*/
#ifndef _MONA_SYSCALL_
#define _MONA_SYSCALL_

#include "global.h"

extern "C" void syscall_entrance(void);
extern "C" uint32_t systemcall_mutex_create();
extern "C" uint32_t systemcall_mutex_lock(uint32_t id);
extern "C" uint32_t systemcall_mutex_unlock(uint32_t id);

#define enter_kernel_lock_mode()    \
    uint32_t __eflags = get_eflags();  \
    disableInterrupt();             \

#define exit_kernel_lock_mode() \
         set_eflags(__eflags);  \


#endif
