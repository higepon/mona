/*!
    \file  kthread.cpp
    \brief kernel thread

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/01 update:$Date$
*/

#include<kthread.h>
#include<monaKernel.h>
#include<monaOperator.h>
#include<monaIhandler.h>

Kthread*    current = NULL; /*< pointer to current kernel thread    */
Kthread*    runlist = NULL; /*< run list                            */
KthreadInfo kthreadInfo;    /*< common thread Information           */

/*!
    \brief init kernel thread

    \author HigePon
    \date   create:2003/03/01 update:
*/
void kthread_init() {

    console->printf("kthread init");

    dword stack = kthread_allocate_stack();

    if (stack == NULL) {
        panic("idle thread:stack allocate error");
    }

    /* create idle thread */
    Kthread* idle = kthread_create_thread(stack, kthread_idle);
    if ( idle == NULL) {
        panic("idle thread:create thread error");
    }

    kthread_add_to_run(idle);

    dword stack2 = kthread_allocate_stack();

    if (stack2 == NULL) {
        panic("idle thread:stack allocate error");
    }

    /* create idle thread */
    Kthread* idle2 = kthread_create_thread(stack2, kthread_idle2);
    if ( idle == NULL) {
        panic("idle thread:create thread error");
    }

    kthread_add_to_run(idle2);
    kthread_printInfo();
}

void kthread_printInfo() {

    console->printf("kthread:runlist[%x][%x][x]\n", runlist, runlist->next, runlist->next->next);

}

/*!
    \brief thread tick

    increment thread tick

    \author HigePon
    \date   create:2003/03/01 update:
*/
void kthread_tick() {

    (kthreadInfo.tick)++;
    (current->tick)++;
    return;
}

/*!
    \brief add thread to run

    \param thread thraed added to run

    \author HigePon
    \date   create:2003/03/01 update:
*/
void kthread_add_to_run(Kthread* thread) {

    /* there is no running thread */
    if (runlist == NULL) {

        runlist = thread;
        runlist->next = NULL;
        return;
    }

    /* add thread at the end */
    Kthread* temp = runlist;
    while (temp->next != NULL) {

        temp = temp->next;
    }

    temp->next = thread;
    thread->next = NULL;
    return;
}

/*!
    \brief remove thread from run

    \param thread thraed removed from run

    \author HigePon
    \date   create:2003/03/01 update:
*/
void kthread_remove_from_run(Kthread* thread) {

    /* do nothing */
    if (runlist == NULL) return;

    /* remove */
    //    runlist = current->next;

    return;
}

#define KTHREAD_STACK_START 0x800000
#define KTHREAD_STACK_END   0x700000
#define KTHREAD_STACK_SIZE  0x2000
/*!
    \brief allcate stack for new thread

    \return stack address. NULL when not enough stack.

    \author HigePon
    \date   create:2003/03/02 update:
*/
dword kthread_allocate_stack() {

    static int allocated = 0;
    dword stack = KTHREAD_STACK_START - KTHREAD_STACK_SIZE * allocated;

    allocated++;

    /* stack not enough */
    if (stack < KTHREAD_STACK_END) return NULL;

    return stack;
}

/*!
    \brief thread yeild

    \author HigePon
    \date   create:2003/03/02 update:
*/
void kthread_yield() {

    /* software interrupt yeild */
}


/*!
    \brief thread yeild

    \param stack stack address
    \param f     entry point

    \return potinter to Kthread. NULL when failed

    \author HigePon
    \date   create:2003/03/02 update:
*/
Kthread* kthread_create_thread(dword stack, void (*f)()) {

    /* create kthread struct */
    Kthread* thread = (Kthread*)malloc(sizeof(Kthread));
    if (thread == NULL) return NULL;

    /* 0 clear */
    memset(thread, 0, sizeof(Kthread));

    /* create thread */
    thread->eip    = (dword)f;
    thread->cs     = 0x08;
    thread->eflags = 0x200046;
    thread->esp    = stack;
    thread->ebp    = stack;

    /* increment thread number */
    (kthreadInfo.threadNum)++;

    return thread;
}


extern "C" void write_font(int a, char b, char c);
extern "C" char pos_x;
extern "C" char pos_y;

/*!
    \brief idle thread

    \author HigePon
    \date   create:2003/03/02 update:
*/
void kthread_idle() {

    dword color = 0;

    while (true) {

        if (current->tick % 50) continue;
        disableInterrupt();

        int x = pos_x;
        int y = pos_y;

        pos_x = 20, pos_y = 20;
        write_font('$', color%2 + 5, 0);
        color++;
        pos_x = x;
        pos_y = y;
        enableInterrupt();
    }
}

/*!
    \brief idle thread

    \author HigePon
    \date   create:2003/03/02 update:
*/
void kthread_idle2() {

    dword color = 0;

    while (true) {

        disableInterrupt();

        int x = pos_x;
        int y = pos_y;

        pos_x = 22, pos_y = 20;
        write_font('$', color%15, 0);
        color++;
        pos_x = x;
        pos_y = y;
        enableInterrupt();
    }
}

/*!
    \brief schedule

    \author HigePon
    \date   create:2003/03/02 update:
*/
void kthread_schedule() {

    /* change runlist */
    Kthread* temp = current;

    kthread_remove_from_run(temp);
    kthread_add_to_run(temp);

    /* change current */
    current = runlist;

    /* switch */
    kthread_switch();
}

/*!
    \brief switch to next

    \author HigePon
    \date   create:2003/03/02 update:
*/
void kthread_switch() {

    arch_kthread_switch();
}



