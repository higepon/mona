/*!
  \file  syscalls.cpp
  \brief syscalls

  Copyright (c) 2002,2003 Higepon
  All rights reserved.
  License=MIT/X Licnese

  \author  HigePon
  \version $Revision$
  \date   create:2003/03/22 update:$Date$
*/

#include<syscalls.h>
#include<userlib.h>
#include<global.h>
#include<tester.h>
#include<io.h>

extern "C" char pos_x;
extern "C" char pos_y;

extern "C" void arch_set_stack_view();
extern "C" void put_pixel(int x, int y, char color);

void syscall_entrance() {

//     int x, y;

//     switch(g_current_process->ebx) {

//     case SYSTEM_CALL_PRINT:

//         while (Semaphore::down(&g_semaphore_console));

//         x = pos_x;
//         y = pos_y;
//         pos_x = 1, pos_y = 27;

//         g_console->printf("user:stdout[%s]", (char*)(g_current_process->esi));

//         pos_x = x;
//         pos_y = y;

//         Semaphore::up(&g_semaphore_console);

//         break;

//     case SYSTEM_CALL_PROCESS_SLEEP:

//         g_process_manager->sleep(g_current_process, g_current_process->esi);

//         /* return code */
//         g_current_process->eax = 0x12345678;

//         break;

//     case SYSTEM_CALL_HEAVEY:

//         enter_kernel_lock_mode();

//         x = pos_x;
//         y = pos_y;

//         pos_x = 1, pos_y = 2;

//         g_console->printf("heavy start\n");

//         pos_x = x;
//         pos_y = y;

//         exit_kernel_lock_mode();

//         enableInterrupt();

//         for (dword i = 0; i < 0xffff; i++) {

//             i++;
//             i--;
//             i++;
//             i--;
//         }

//         enter_kernel_lock_mode();

//         x = pos_x;
//         y = pos_y;

//         pos_x = 2, pos_y = 4;

//         g_console->printf("heavy end\n");

//         pos_x = x;
//         pos_y = y;


//         /* return code */
//         g_current_process->eax = 0x12345678;

//         exit_kernel_lock_mode();

//         break;
//     case SYSTEM_CALL_KILL:

//         g_process_manager->kill(g_current_process);
//         break;

//     case SYSTEM_CALL_PUT_PIXEL:

//         put_pixel((int)(g_current_process->esi), (int)(g_current_process->ecx), (char)(g_current_process->edi));
//         g_current_process->eax = 0;
//         break;

//     case SYSTEM_CALL_SEND:

//         g_current_process->eax = send((char*)(g_current_process->esi), (Message*)(g_current_process->ecx));
//         break;

//     case SYSTEM_CALL_RECEIVE:

//         g_current_process->eax = receive((Message*)(g_current_process->esi));
//         break;

//     default:
//         g_console->printf("syscall:default");
//         break;
//     }
    return;
}
