#include<monaKernel.h>
#include<kthread.h>
#include<disp.h>
#include<io.h>
#include<pic.h>
#include<semaphore.h>
#include<global.h>

extern "C" void write_font(int a, char b, char c);
extern "C" char pos_x;
extern "C" char pos_y;

static semaphore sem = 1;

/*!
    \brief idle thread

    \author HigePon
    \date   create:2003/03/02 update:
*/
void disp_name1() {

    enableTimer();
    while (true) {
        //    g_console->printf("demo1 = %d, demo2 = %d demo3 = %d\n", g_kthreadInfo.demo1, g_kthreadInfo.demo2, g_kthreadInfo.demo2);
       g_console->printf("d");
//          asm volatile ("mov $0x12345678, %eax \n"
//                        "mov $0x23456789, %ebx \n"
//                        "mov $0x34567890, %ecx \n"
//                        "mov $0x45678901, %edx \n"
//                        );

        (g_kthreadInfo.demo1)++;
    }
}

/*!
    \brief idle thread

    \author HigePon
    \date   create:2003/03/02 update:
*/
void disp_name2() {

    enableTimer();
    while (true) {
        //      g_console->printf("[2][%x]", get_eflags());
        (g_kthreadInfo.demo2)++;
    }
}

void disp_name3() {

    enableTimer();
    while (true) {
        (g_kthreadInfo.demo3)++;
    }
}
void disp_name4() {

    enableTimer();
    while (true) {
        asm volatile("nop");
    }
}
