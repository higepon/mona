/*!
    \file  syscalls.cpp
    \brief syscalls

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/22 update:$Date$
*/

#include<syscalls.h>
#include<global.h>
#include<kthread.h>
#include<io.h>

void syscall_entrance() {

    outportb(0x20, 0x20);

    switch(g_kthread_current->ebx) {


      case KTHREAD_YIELD:
	  //          g_console->printf("syscall:kthread_yield");
          kthread_yield();
          break;

      default:
          g_console->printf("syscall:default");
          break;
    }

    return;
}
