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
#include<io.h>

void syscall_entrance() {

    outportb(0x20, 0x20);

    switch(g_current_process->ebx) {


      case 0:
          break;

      default:
          g_console->printf("syscall:default");
          break;
    }

    return;
}
