/*!
    \file  syscalls.h
    \brief syscalls

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/22 update:$Date$
*/
#ifndef _MONA_SYSCALL_
#define _MONA_SYSCALL_

#include <global.h>

extern "C" void syscall_entrance(void);

#define enter_kernel_lock_mode()    \
    dword __eflags = get_eflags();  \
    disableInterrupt();             \

#define exit_kernel_lock_mode() \
         set_eflags(__eflags);    \


#endif
