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
	g_console->printf("[1][%x]", get_eflags());
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
	g_console->printf("[2][%x]", get_eflags());
	(g_kthreadInfo.demo2)++;
    }
}

void disp_name3() {

    enableTimer();
    while (true) {
        asm volatile("nop");
    }
}
void disp_name4() {

    enableTimer();
    while (true) {
        asm volatile("nop");
    }
}
