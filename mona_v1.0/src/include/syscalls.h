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

#define SYSTEM_CALL_PROCESS_SLEEP 5

extern "C" void syscall_entrance(void);

void syscall_sleep(dword tick);

#endif
